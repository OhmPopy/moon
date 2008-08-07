/*
 * shape.cpp: This match the classes inside System.Windows.Shapes
 *
 * Contact:
 *   Moonlight List (moonlight-list@lists.ximian.com)
 *
 * Copyright 2007-2008 Novell, Inc. (http://www.novell.com)
 *
 * See the LICENSE file included with the distribution for details.
 * 
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cairo.h>

#include <math.h>

#include "runtime.h"
#include "shape.h"
#include "brush.h"
#include "array.h"
#include "utils.h"

//
// SL-Cairo convertion and helper routines
//

#define EXACT_BOUNDS 1

static cairo_line_join_t
convert_line_join (PenLineJoin pen_line_join)
{
	switch (pen_line_join) {
	default:
		/* note: invalid values should be trapped in SetValue (see bug #340799) */
		g_warning ("Invalid value (%d) specified for PenLineJoin, using default.", pen_line_join);
		/* at this stage we use the default value (Miter) for Shape */
	case PenLineJoinMiter:
		return CAIRO_LINE_JOIN_MITER;
	case PenLineJoinBevel:
		return CAIRO_LINE_JOIN_BEVEL;
	case PenLineJoinRound:
		return CAIRO_LINE_JOIN_ROUND;
	}
}

/* NOTE: Triangle doesn't exist in Cairo - unless you patched it using https://bugzilla.novell.com/show_bug.cgi?id=345892 */
#ifndef HAVE_CAIRO_LINE_CAP_TRIANGLE
	#define CAIRO_LINE_CAP_TRIANGLE		 CAIRO_LINE_CAP_ROUND
#endif

static cairo_line_cap_t
convert_line_cap (PenLineCap pen_line_cap)
{
	switch (pen_line_cap) {
	default:
		/* note: invalid values should be trapped in SetValue (see bug #340799) */
		g_warning ("Invalid value (%d) specified for PenLineCap, using default.", pen_line_cap);
		/* at this stage we use the default value (Flat) for Shape */
	case PenLineCapFlat:
		return CAIRO_LINE_CAP_BUTT;
	case PenLineCapSquare:
		return CAIRO_LINE_CAP_SQUARE;
	case PenLineCapRound:
		return CAIRO_LINE_CAP_ROUND;
	case PenLineCapTriangle: 		
		return CAIRO_LINE_CAP_TRIANGLE;
	}
}

cairo_fill_rule_t
convert_fill_rule (FillRule fill_rule)
{
	switch (fill_rule) {
	default:
		/* note: invalid values should be trapped in SetValue (see bug #340799) */
		g_warning ("Invalid value (%d) specified for FillRule, using default.", fill_rule);
		/* at this stage we use the default value (EvenOdd) for Geometry */
	case FillRuleEvenOdd:
		return CAIRO_FILL_RULE_EVEN_ODD;
	case FillRuleNonzero:
		return CAIRO_FILL_RULE_WINDING;
	}
}

//
// Shape
//

DependencyProperty *Shape::FillProperty;
DependencyProperty *Shape::StretchProperty;
DependencyProperty *Shape::StrokeProperty;
DependencyProperty *Shape::StrokeDashArrayProperty;
DependencyProperty *Shape::StrokeDashCapProperty;
DependencyProperty *Shape::StrokeDashOffsetProperty;
DependencyProperty *Shape::StrokeEndLineCapProperty;
DependencyProperty *Shape::StrokeLineJoinProperty;
DependencyProperty *Shape::StrokeMiterLimitProperty;
DependencyProperty *Shape::StrokeStartLineCapProperty;
DependencyProperty *Shape::StrokeThicknessProperty;

Shape::Shape ()
{
	stroke = NULL;
	fill = NULL;
	path = NULL;
	origin = Point (0, 0);
	cached_surface = NULL;
	SetShapeFlags (UIElement::SHAPE_NORMAL);
	cairo_matrix_init_identity (&stretch_transform);
}

Shape::~Shape ()
{
	// That also destroys the cached surface
	InvalidatePathCache (true);
}

bool
Shape::MixedHeightWidth (Value **height, Value **width)
{
	Value *vw = GetValueNoDefault (FrameworkElement::WidthProperty);
	Value *vh = GetValueNoDefault (FrameworkElement::HeightProperty);

	// nothing is drawn if only the width or only the height is specified
	if ((!vw && vh) || (vw && !vh)) {
		SetShapeFlags (UIElement::SHAPE_EMPTY);
		return true;
	}

	if (width) *width = vw;
	if (height) *height = vh;
	return false;
}

void
Shape::Draw (cairo_t *cr)
{
	if (!path || (path->cairo.num_data == 0))
		BuildPath ();

	cairo_save (cr);
	cairo_transform (cr, &stretch_transform);

	cairo_new_path (cr);
	cairo_append_path (cr, &path->cairo);

	cairo_restore (cr);
}

// break up operations so we can exclude optional stuff, like:
// * StrokeStartLineCap & StrokeEndLineCap
// * StrokeLineJoin & StrokeMiterLimit
// * Fill

bool
Shape::SetupLine (cairo_t *cr)
{
	double thickness = GetStrokeThickness ();
	
	// check if something will be drawn or return 
	// note: override this method if cairo is used to compute bounds
	if (thickness == 0)
		return false;

	cairo_set_line_width (cr, thickness);

	return SetupDashes (cr, thickness);
}

bool
Shape::SetupDashes (cairo_t *cr, double thickness)
{
	return Shape::SetupDashes (cr, thickness, GetStrokeDashOffset () * thickness);
}

bool
Shape::SetupDashes (cairo_t *cr, double thickness, double offset)
{
	int count = 0;
	double *dashes = GetStrokeDashArray (&count);
	if (dashes && (count > 0)) {
		// NOTE: special case - if we continue cairo will stops drawing!
		if ((count == 1) && (*dashes == 0.0))
			return false;

		// multiply dashes length with thickness
		double *dmul = new double [count];
		for (int i=0; i < count; i++)
			dmul [i] = dashes [i] * thickness;

		cairo_set_dash (cr, dmul, count, offset);
		delete [] dmul;
	} else {
		cairo_set_dash (cr, NULL, 0, 0.0);
	}
	return true;
}

void
Shape::SetupLineCaps (cairo_t *cr)
{
	// Setting the cap to dash_cap. the endcaps (if different) are handled elsewhere
	PenLineCap cap = GetStrokeDashCap ();
	
	cairo_set_line_cap (cr, convert_line_cap (cap));
}

void
Shape::SetupLineJoinMiter (cairo_t *cr)
{
	PenLineJoin join = GetStrokeLineJoin ();
	double limit = GetStrokeMiterLimit ();
	
	cairo_set_line_join (cr, convert_line_join (join));
	cairo_set_miter_limit (cr, limit);
}

// returns true if the path is set on the cairo, false if not
bool
Shape::Fill (cairo_t *cr, bool do_op)
{
	if (!fill)
		return false;

	Draw (cr);
	if (do_op) {
		fill->SetupBrush (cr, this);
		cairo_set_fill_rule (cr, convert_fill_rule (GetFillRule ()));
		cairo_fill_preserve (cr);
	}
	return true;
}

Rect
Shape::ComputeStretchBounds (Rect shape_bounds)
{
	Value *vh, *vw;
	needs_clip = true;

	/*
	 * NOTE: this code is extremely fragile don't make a change here without
	 * checking the results of the test harness on with MOON_DRT_CATEGORIES=stretch
	 */

	if (Shape::MixedHeightWidth (&vh, &vw)) {
		return shape_bounds;
	}

	double w = vw ? vw->AsDouble () : 0.0;
	double h = vh ? vh->AsDouble () : 0.0;

	if ((h < 0.0) || (w < 0.0)) {
		SetShapeFlags (UIElement::SHAPE_EMPTY);
		return shape_bounds;
	}

	if ((vh && (h <= 0.0)) || (vw && (w <= 0.0))) { 
		SetShapeFlags (UIElement::SHAPE_EMPTY);
		return shape_bounds;
	}

	h = (h == 0.0) ? shape_bounds.h : h;
	w = (w == 0.0) ? shape_bounds.w : w;

	if (h <= 0.0 || w <= 0.0 || shape_bounds.w <= 0.0 || shape_bounds.h <= 0.0) {
		SetShapeFlags (UIElement::SHAPE_EMPTY);
		return shape_bounds;
	}

	Stretch stretch = GetStretch ();
	if (stretch != StretchNone) {
		Rect logical_bounds = ComputeShapeBounds (true);

		bool adj_x = logical_bounds.w != 0.0;
		bool adj_y = logical_bounds.h != 0.0;
             
		double diff_x = shape_bounds.w - logical_bounds.w;
		double diff_y = shape_bounds.h - logical_bounds.h;
		double sw = adj_x ? (w - diff_x) / logical_bounds.w : 1.0;
		double sh = adj_y ? (h - diff_y) / logical_bounds.h : 1.0;

		bool center = false;

		switch (stretch) {
		case StretchFill:
			needs_clip = false;
			center = true;
			break;
		case StretchUniform:
			needs_clip = false;
			sw = sh = (sw < sh) ? sw : sh;
			center = true;
			break;
		case StretchUniformToFill:
			sw = sh = (sw > sh) ? sw : sh;
			break;
		case StretchNone:
			/* not reached */
		break;
		}

		// trying to avoid the *VERY*SLOW* adjustments
		// e.g. apps like Silverlight World have a ratio up to 50 unneeded for 1 needed adjustment
		// so it all boilds down are we gonna change bounds anyway ?
		#define IS_SIGNIFICANT(dx,x)	(IS_ZERO(dx) && (fabs(dx) * x - x > 1.0))
		if ((adj_x && IS_SIGNIFICANT((sw - 1), shape_bounds.w)) || (adj_y && IS_SIGNIFICANT((sh - 1), shape_bounds.h))) {
			// FIXME: this IS still UBER slow
			// hereafter we're doing a second pass to refine the sw and sh we guessed
			// the first time. This usually gives pixel-recise stretches for Paths
			cairo_matrix_t temp;
			cairo_matrix_init_scale (&temp, adj_x ? sw : 1.0, adj_y ? sh : 1.0);
			Rect extents = ComputeShapeBounds (false, &temp);
			if (extents.w != shape_bounds.w && extents.h != shape_bounds.h) {
				sw *= adj_x ? (w - extents.w + logical_bounds.w * sw) / (logical_bounds.w * sw): 1.0;
				sh *= adj_y ? (h - extents.h + logical_bounds.h * sh) / (logical_bounds.h * sh): 1.0;

				switch (stretch) {
				case StretchUniform:
					sw = sh = (sw < sh) ? sw : sh;
					break;
				case StretchUniformToFill:
					sw = sh = (sw > sh) ? sw : sh;
					break;
				default:
					break;
				}
			}
			// end of the 2nd pass code
		}

		double x = vh || adj_x ? shape_bounds.x : 0;
		double y = vw || adj_y ? shape_bounds.y : 0;
		if (center)
			cairo_matrix_translate (&stretch_transform, 
						adj_x ? w * 0.5 : 0, 
						adj_y ? h * 0.5 : 0);
		else //UniformToFill
			cairo_matrix_translate (&stretch_transform, 
						adj_x ? (logical_bounds.w * sw + diff_x) * .5 : 0,
						adj_y ? (logical_bounds.h * sh + diff_y) * .5: 0);
		
		cairo_matrix_scale (&stretch_transform, 
				    adj_x ? sw : 1.0, 
				    adj_y ? sh : 1.0);
		
		cairo_matrix_translate (&stretch_transform, 
					adj_x ? -shape_bounds.w * 0.5 : 0, 
					adj_y ? -shape_bounds.h * 0.5 : 0);

		if (!Is (Type::LINE) || (vh && vw))
			cairo_matrix_translate (&stretch_transform, -x, -y);
		
		// Double check our math
		cairo_matrix_t test = stretch_transform;
		if (cairo_matrix_invert (&test)) {
			g_warning ("Unable to compute stretch transform %f %f %f %f \n", sw, sh, shape_bounds.x, shape_bounds.y);
		}		
	}
	
	shape_bounds = shape_bounds.Transform (&stretch_transform);
	
	if (vh && vw) {
		Rect reduced_bounds = shape_bounds.Intersection (Rect (0, 0, vw->AsDouble (), vh->AsDouble ()));
		needs_clip = reduced_bounds != shape_bounds;
		needs_clip = needs_clip && stretch != StretchFill;
		needs_clip = needs_clip && stretch != StretchUniform;
	}
	
	return shape_bounds;
}

void
Shape::Stroke (cairo_t *cr, bool do_op)
{
	if (do_op) {
		stroke->SetupBrush (cr, this);
		cairo_stroke (cr);
	}
}

void
Shape::Clip (cairo_t *cr)
{
	// some shapes, like Line, Polyline, Polygon and Path, are clipped if both Height and Width properties are present
	if (needs_clip) {
		Value *vh = GetValueNoDefault (FrameworkElement::HeightProperty);
		if (!vh)
			return;
		
		Value *vw = GetValueNoDefault (FrameworkElement::WidthProperty);
		if (!vw)
			return;

#if EXACT_CLIP
		cairo_rectangle (cr, 0, 0, vw->AsDouble (), vh->AsDouble ());
#else
		cairo_rectangle (cr, 0, 0, vw->AsDouble () > 1 ? vw->AsDouble () : 1, vh->AsDouble () > 1 ? vh->AsDouble() : 1);
#endif
		cairo_clip (cr);
		cairo_new_path (cr);
	}
}

//
// Returns TRUE if surface is a good candidate for caching.
// We accept a little bit of scaling.
//
bool
Shape::IsCandidateForCaching (void)
{
	if (IsEmpty ()) 
		return FALSE;

	if (! GetSurface ())
		return FALSE;

	// This is not 100% correct check -- the actual surface size might be
	// a tiny little bit larger. It's not a problem though if we go few
	// bytes above the cache limit.
	if (!GetSurface ()->VerifyWithCacheSizeCounter ((int) bounds.w, (int) bounds.h))
		return FALSE;

	// one last line of defense, lets not cache things 
	// much larger than the screen.
	if (bounds.w * bounds.h > 4000000)
		return FALSE;

	return TRUE;
}

//
// This routine is useful for Shape derivatives: it can be used
// to either get the bounding box from cairo, or to paint it
//
void
Shape::DoDraw (cairo_t *cr, bool do_op)
{
	bool ret = FALSE;

	// quick out if, when building the path, we detected an empty shape
	if (IsEmpty ())
		goto cleanpath;

	if (do_op && cached_surface == NULL && IsCandidateForCaching ()) {
		Rect cache_extents = bounds.RoundOut ();
		cairo_t *cached_cr = NULL;
		
		// g_warning ("bounds (%f, %f), extents (%f, %f), cache_extents (%f, %f)", 
		// bounds.w, bounds.h,
		// extents.w, extents.h,
		// cache_extents.w, cache_extents.h);
		
		cached_surface = image_brush_create_similar (cr, (int) cache_extents.w, (int) cache_extents.h);
		cairo_surface_set_device_offset (cached_surface, -cache_extents.x, -cache_extents.y);
		cached_cr = cairo_create (cached_surface);
		
		cairo_set_matrix (cached_cr, &absolute_xform);
		Clip (cached_cr);
		
		ret = DrawShape (cached_cr, do_op);
		
		cairo_destroy (cached_cr);
		
		// Increase our cache size
		cached_size = GetSurface ()->AddToCacheSizeCounter ((int) cache_extents.w, (int) cache_extents.h);
	}
	
	if (do_op && cached_surface) {
		cairo_pattern_t *cached_pattern = NULL;

		cached_pattern = cairo_pattern_create_for_surface (cached_surface);
		cairo_identity_matrix (cr);
		cairo_set_source (cr, cached_pattern);
		cairo_pattern_destroy (cached_pattern);
		cairo_paint (cr);
	} else {
		cairo_set_matrix (cr, &absolute_xform);
		Clip (cr);
		
		if (DrawShape (cr, do_op))
			return;
	}

cleanpath:
	if (do_op)
		cairo_new_path (cr);
}

void
Shape::Render (cairo_t *cr, int x, int y, int width, int height)
{
	cairo_save (cr);
	DoDraw (cr, true);
	cairo_restore (cr);
}

Point
Shape::ComputeOriginPoint (Rect shape_bounds)
{
	return Point (shape_bounds.x, shape_bounds.y);
}

void
Shape::ShiftPosition (Point p)
{
	if (cached_surface) {
		cairo_surface_set_device_offset (cached_surface, -p.x, -p.y);
	}

	FrameworkElement::ShiftPosition (p);
}

void
Shape::ComputeBounds ()
{
	cairo_matrix_init_identity (&stretch_transform);
	InvalidateSurfaceCache ();
	
	extents = ComputeShapeBounds (false);

	extents = ComputeStretchBounds (extents);
	origin = ComputeOriginPoint (extents);

	bounds = IntersectBoundsWithClipPath (extents, false).Transform (&absolute_xform);
	//printf ("%f,%f,%f,%f\n", bounds.x, bounds.y, bounds.w, bounds.h);
}

Rect
Shape::ComputeShapeBounds (bool logical)
{
	if (!path || (path->cairo.num_data == 0))
		BuildPath ();

	if (IsEmpty () || Shape::MixedHeightWidth (NULL, NULL))
		return Rect ();

	double thickness = (logical || !IsStroked ()) ? 0.0 : GetStrokeThickness ();
	
	cairo_t *cr = measuring_context_create ();
	cairo_set_line_width (cr, thickness);

	if (thickness > 0.0) {
		//FIXME: still not 100% precise since it could be different from the end cap
		PenLineCap cap = GetStrokeStartLineCap ();
		if (cap == PenLineCapFlat)
			cap = GetStrokeEndLineCap ();
		cairo_set_line_cap (cr, convert_line_cap (cap));
	}

	cairo_append_path (cr, &path->cairo);
	
	double x1, y1, x2, y2;

	if (logical) {
		cairo_path_extents (cr, &x1, &y1, &x2, &y2);
	} else if (thickness > 0) {
		cairo_stroke_extents (cr, &x1, &y1, &x2, &y2);
	} else {
		cairo_fill_extents (cr, &x1, &y1, &x2, &y2);
	}

	Rect bounds = Rect (MIN (x1, x2), MIN (y1, y2), fabs (x2 - x1), fabs (y2 - y1));

	measuring_context_destroy (cr);

	return bounds;
}

Rect
Shape::ComputeLargestRectangleBounds ()
{
	Rect largest = ComputeLargestRectangle ();
	if (largest.IsEmpty ())
		return largest;

	return IntersectBoundsWithClipPath (largest, false).Transform (&absolute_xform);
}

Rect
Shape::ComputeLargestRectangle ()
{
	// by default the largest rectangle that fits into a shape is empty
	return Rect ();
}

void
Shape::GetSizeForBrush (cairo_t *cr, double *width, double *height)
{
	*height = extents.h;
	*width = extents.w;
}

bool
Shape::InsideObject (cairo_t *cr, double x, double y)
{
	cairo_save (cr);

	bool ret = true;

	uielement_transform_point (this, &x ,&y);
	
	// cairo_in_* functions which we're using to check if point inside
	// the path don't take the clipping into account. Therefore, we need 
	// to do this in two steps: first check if the point is within 
	// the clipping bounds and later check if within the path itself.

	Value *clip_geometry = GetValue (UIElement::ClipProperty);
	if (clip_geometry) {
		Geometry *clip = clip_geometry->AsGeometry ();
		if (clip) {
			clip->Draw (NULL, cr);
			ret = cairo_in_fill (cr, x, y);
			cairo_new_path (cr);

			if (!ret) {
				cairo_restore (cr);
				return false;
			}
		}
	}

	// don't do the operation but do consider filling
	DoDraw (cr, false);

	// don't check in_stroke without a stroke or in_fill without a fill (even if it can be filled)
	ret = ((stroke && cairo_in_stroke (cr, x, y)) || (fill && CanFill () && cairo_in_fill (cr, x, y)));

	cairo_new_path (cr);
	cairo_restore (cr);
		
	return ret;
}

void
Shape::CacheInvalidateHint (void)
{
	// Also kills the surface cache
	InvalidatePathCache ();
}

void
Shape::OnPropertyChanged (PropertyChangedEventArgs *args)
{
	if (args->property->GetOwnerType() != Type::SHAPE) {
		if ((args->property == FrameworkElement::HeightProperty) || (args->property == FrameworkElement::WidthProperty))
			InvalidatePathCache ();

		if (args->property == UIElement::OpacityProperty) {
			if (IS_INVISIBLE (args->new_value->AsDouble ()))
				InvalidateSurfaceCache ();
		} else {
			if (args->property == UIElement::VisibilityProperty) {
				if (args->new_value->AsInt32() != VisibilityVisible)
					InvalidateSurfaceCache ();
			}
		}

		FrameworkElement::OnPropertyChanged (args);
		return;
	}

	if (args->property == Shape::StretchProperty) {
		InvalidatePathCache ();
		UpdateBounds (true);
	}
	else if (args->property == Shape::StrokeProperty) {
		Brush *new_stroke = args->new_value ? args->new_value->AsBrush () : NULL;

		if (!stroke || !new_stroke) {
			// If the stroke changes from null to
			// <something> or <something> to null, then
			// some shapes need to reclaculate the offset
			// (based on stroke thickness) to start
			// painting.
			InvalidatePathCache ();
               } else
			InvalidateSurfaceCache ();
		
		stroke = new_stroke;
		UpdateBounds ();
	} else if (args->property == Shape::FillProperty) {
		fill = args->new_value ? args->new_value->AsBrush() : NULL;
		InvalidateSurfaceCache ();
		UpdateBounds ();
	} else if (args->property == Shape::StrokeThicknessProperty) {
		InvalidatePathCache ();
		UpdateBounds ();
	} else if (args->property == Shape::StrokeDashCapProperty
		   || args->property == Shape::StrokeEndLineCapProperty
		   || args->property == Shape::StrokeLineJoinProperty
		   || args->property == Shape::StrokeMiterLimitProperty
		   || args->property == Shape::StrokeStartLineCapProperty) {
		UpdateBounds ();
		InvalidatePathCache ();
	}
	
	Invalidate ();

	NotifyListenersOfPropertyChange (args);
}

void
Shape::OnSubPropertyChanged (DependencyProperty *prop, DependencyObject *obj, PropertyChangedEventArgs *subobj_args)
{
	if (prop == Shape::FillProperty || prop == Shape::StrokeProperty) {
		Invalidate ();
		InvalidateSurfaceCache ();
	}
	else
		FrameworkElement::OnSubPropertyChanged (prop, obj, subobj_args);
}

Point
Shape::GetTransformOrigin ()
{
	Point user_xform_origin = GetRenderTransformOrigin ();
	
	return Point (GetWidth () * user_xform_origin.x, 
		      GetHeight () * user_xform_origin.y);
}

void
Shape::InvalidatePathCache (bool free)
{
	SetShapeFlags (UIElement::SHAPE_NORMAL);
	if (path) {
		if (free) {
			moon_path_destroy (path);
			path = NULL;
		} else {
			moon_path_clear (path);
		}
	}

	InvalidateSurfaceCache ();
}

void
Shape::InvalidateSurfaceCache (void)
{
	if (cached_surface) {
		cairo_surface_destroy (cached_surface);
		if (GetSurface ())
			GetSurface ()->RemoveFromCacheSizeCounter (cached_size);
		cached_surface = NULL;
		cached_size = 0;
	}
}

void
Shape::SetFill (Brush *fill)
{
	SetValue (Shape::FillProperty, Value (fill));
}

Brush *
Shape::GetFill ()
{
	Value *value = GetValue (Shape::FillProperty);
	
	return value ? value->AsBrush () : NULL;
}

void
Shape::SetStroke (Brush *stroke)
{
	SetValue (Shape::StrokeProperty, Value (stroke));
}

Brush *
Shape::GetStroke ()
{
	Value *value = GetValue (Shape::StrokeProperty);
	
	return value ? value->AsBrush () : NULL;
}

void
Shape::SetStretch (Stretch stretch)
{
	SetValue (Shape::StretchProperty, Value (stretch));
}

Stretch
Shape::GetStretch ()
{
	return (Stretch) GetValue (Shape::StretchProperty)->AsInt32 ();
}

void
Shape::SetStrokeDashArray (double *dashes, int n)
{
	SetValue (Shape::StrokeDashArrayProperty, Value (dashes, n));
}

/*
 * note: We return a reference, not a copy, of the dashes. Not a big issue as
 * Silverlight Shape.StrokeDashArray only has a setter (no getter), so it's 
 * use is only internal.
 */
double *
Shape::GetStrokeDashArray (int *n)
{
	Value *value = GetValue (Shape::StrokeDashArrayProperty);
	
	if (!value) {
		*n = 0;
		return NULL;
	}
	
	DoubleArray *array = value->AsDoubleArray ();
	*n = array->basic.count;
	
	return array->values;
}

void
Shape::SetStrokeDashCap (PenLineCap cap)
{
	SetValue (Shape::StrokeDashCapProperty, Value (cap));
}

PenLineCap
Shape::GetStrokeDashCap ()
{
	return (PenLineCap) GetValue (Shape::StrokeDashCapProperty)->AsInt32 ();
}

void
Shape::SetStrokeDashOffset (double offset)
{
	SetValue (Shape::StrokeDashOffsetProperty, Value (offset));
}

double
Shape::GetStrokeDashOffset ()
{
	return GetValue (Shape::StrokeDashOffsetProperty)->AsDouble ();
}

void
Shape::SetStrokeEndLineCap (PenLineCap cap)
{
	SetValue (Shape::StrokeEndLineCapProperty, Value (cap));
}

PenLineCap
Shape::GetStrokeEndLineCap ()
{
	return (PenLineCap) GetValue (Shape::StrokeEndLineCapProperty)->AsInt32 ();
}

void
Shape::SetStrokeLineJoin (PenLineJoin join)
{
	SetValue (Shape::StrokeLineJoinProperty, Value (join));
}

PenLineJoin
Shape::GetStrokeLineJoin ()
{
	return (PenLineJoin) GetValue (Shape::StrokeLineJoinProperty)->AsInt32 ();
}

void
Shape::SetStrokeMiterLimit (double limit)
{
	SetValue (Shape::StrokeMiterLimitProperty, Value (limit));
}

double
Shape::GetStrokeMiterLimit ()
{
	return GetValue (Shape::StrokeMiterLimitProperty)->AsDouble ();
}

void
Shape::SetStrokeStartLineCap (PenLineCap cap)
{
	SetValue (Shape::StrokeStartLineCapProperty, Value (cap));
}

PenLineCap
Shape::GetStrokeStartLineCap ()
{
	return (PenLineCap) GetValue (Shape::StrokeStartLineCapProperty)->AsInt32 ();
}

void
Shape::SetStrokeThickness (double thickness)
{
	SetValue (Shape::StrokeThicknessProperty, Value (thickness));
}

double
Shape::GetStrokeThickness ()
{
	return GetValue (Shape::StrokeThicknessProperty)->AsDouble ();
}


Brush *
shape_get_fill (Shape *shape)
{
	return shape->GetFill ();
}

void 
shape_set_fill (Shape *shape, Brush *fill)
{
	shape->SetFill (fill);
}

Brush *
shape_get_stroke (Shape *shape)
{
	return shape->GetStroke ();
}

void 
shape_set_stroke (Shape *shape, Brush *stroke)
{
	shape->SetStroke (stroke);
}

Stretch
shape_get_stretch (Shape *shape)
{
	return shape->GetStretch ();
}

void
shape_set_stretch (Shape *shape, Stretch stretch)
{
	shape->SetStretch (stretch);
}

void
shape_set_stroke_dash_array (Shape *shape, double *dashes, int n)
{
	shape->SetStrokeDashArray (dashes, n);
}

PenLineCap
shape_get_stroke_dash_cap (Shape *shape)
{
	return shape->GetStrokeDashCap ();
}

void
shape_set_stroke_dash_cap (Shape *shape, PenLineCap cap)
{
	shape->SetStrokeDashCap (cap);
}

double
shape_get_stroke_dash_offset (Shape *shape)
{
	return shape->GetValue (Shape::StrokeDashOffsetProperty)->AsDouble ();
}

void
shape_set_stroke_dash_offset (Shape *shape, double offset)
{
	shape->SetStrokeDashOffset (offset);
}

PenLineCap
shape_get_stroke_end_line_cap (Shape *shape)
{
	return shape->GetStrokeEndLineCap ();
}

void
shape_set_stroke_end_line_cap (Shape *shape, PenLineCap cap)
{
	shape->SetStrokeEndLineCap (cap);
}

PenLineJoin
shape_get_stroke_line_join (Shape *shape)
{
	return shape->GetStrokeLineJoin ();
}

void
shape_set_stroke_line_join (Shape *shape, PenLineJoin join)
{
	shape->SetStrokeLineJoin (join);
}

double
shape_get_stroke_miter_limit (Shape *shape)
{
	return shape->GetStrokeMiterLimit ();
}

void
shape_set_stroke_miter_limit (Shape *shape, double limit)
{
	shape->SetStrokeMiterLimit (limit);
}

PenLineCap
shape_get_stroke_start_line_cap (Shape *shape)
{
	return shape->GetStrokeStartLineCap ();
}

void
shape_set_stroke_start_line_cap (Shape *shape, PenLineCap cap)
{
	shape->SetStrokeStartLineCap (cap);
}

double
shape_get_stroke_thickness (Shape *shape)
{
	return shape->GetStrokeThickness ();
}

void
shape_set_stroke_thickness (Shape *shape, double thickness)
{
	shape->SetStrokeThickness (thickness);
}



//
// Ellipse
//

Ellipse::Ellipse ()
{
	SetStretch (StretchFill);
}

/*
 * Ellipses (like Rectangles) are special and they don't need to participate
 * in the other stretch logic
 */
Rect
Ellipse::ComputeStretchBounds (Rect shape_bounds)
{
	needs_clip = !IsDegenerate () && (GetStretch () == StretchUniformToFill);
	return shape_bounds;
}

Rect
Ellipse::ComputeShapeBounds (bool logical)
{
	Value *vh, *vw;

	if (logical)
		return extents;

	if (Shape::MixedHeightWidth (&vh, &vw)) {
		SetShapeFlags (UIElement::SHAPE_EMPTY);
		return Rect ();
	}

	double w = GetWidth ();
	double h = GetHeight ();
	if ((vh && (h <= 0.0)) || (vw && (w <= 0.0))) { 
		SetShapeFlags (UIElement::SHAPE_EMPTY);
		return Rect ();
	}

	double t = IsStroked () ? GetStrokeThickness () : 0.0;
	return Rect (0, 0, MAX (w, t), MAX (h, t));
}

// The Ellipse shape can be drawn while ignoring properties:
// * Shape::StrokeStartLineCap
// * Shape::StrokeEndLineCap
// * Shape::StrokeLineJoin
// * Shape::StrokeMiterLimit
bool
Ellipse::DrawShape (cairo_t *cr, bool do_op)
{
	bool drawn = Fill (cr, do_op);

	if (!stroke)
		return drawn;
	if (!SetupLine (cr))
		return drawn;
	SetupLineCaps (cr);

	if (!drawn)
		Draw (cr);
	Stroke (cr, do_op);
	return true; 
}

void
Ellipse::BuildPath ()
{
	Value *height, *width;
	
	if (Shape::MixedHeightWidth (&height, &width))
		return;

	Stretch stretch = GetStretch ();
	double t = IsStroked () ? GetStrokeThickness () : 0.0;
	Rect rect = Rect (0.0, 0.0, GetWidth (), GetHeight ());

	if (rect.w < 0.0 || rect.h < 0.0) {
		SetShapeFlags (UIElement::SHAPE_EMPTY);		
		return;
	}

	switch (stretch) {
	case StretchNone:
		rect.w = rect.h = 0.0;
		break;
	case StretchUniform:
		rect.w = rect.h = (rect.w < rect.h) ? rect.w : rect.h;
		break;
	case StretchUniformToFill:
		rect.w = rect.h = (rect.w > rect.h) ? rect.w : rect.h;
		break;
	case StretchFill:
		/* nothing needed here.  the assignment of w/h above
		   is correct for this case. */
		break;
	}

	if (rect.w <= t || rect.h <= t){
		rect.w = MAX (rect.w, t + t * 0.001);
		rect.h = MAX (rect.h, t + t * 0.001);
		SetShapeFlags (UIElement::SHAPE_DEGENERATE);
	} else
		SetShapeFlags (UIElement::SHAPE_NORMAL);

	rect = rect.GrowBy ( -t/2, -t/2);

	path = moon_path_renew (path, MOON_PATH_ELLIPSE_LENGTH);
	moon_ellipse (path, rect.x, rect.y, rect.w, rect.h);
}

Rect
Ellipse::ComputeLargestRectangle ()
{
	double t = GetStrokeThickness ();
	double x = (GetWidth () - t) * cos (M_PI_2);
	double y = (GetHeight () - t) * sin (M_PI_2);
	return ComputeShapeBounds (false).GrowBy (-x, -y).RoundIn ();
}

void
Ellipse::OnPropertyChanged (PropertyChangedEventArgs *args)
{
	DependencyProperty *prop = args->property;

	if ((prop == Shape::StrokeThicknessProperty) || (prop == Shape::StretchProperty) ||
		(prop == FrameworkElement::WidthProperty) || (prop == FrameworkElement::HeightProperty)) {
		BuildPath ();
		InvalidateSurfaceCache ();
	}

	// Ellipse has no property of it's own
	Shape::OnPropertyChanged (args);
}

//
// Rectangle
//

DependencyProperty *Rectangle::RadiusXProperty;
DependencyProperty *Rectangle::RadiusYProperty;

Rectangle::Rectangle ()
{
	SetStretch (StretchFill);
}

/*
 * Rectangles (like Ellipses) are special and they don't need to participate
 * in the other stretch logic
 */
Rect
Rectangle::ComputeStretchBounds (Rect shape_bounds)
{
	needs_clip = !IsDegenerate () && (GetStretch () == StretchUniformToFill);
	return shape_bounds;
}

Rect
Rectangle::ComputeShapeBounds (bool logical)
{
	// this base version of ComputeShapeBounds does not need to distinguish between
	// logical and physical bounds and we know that physical is already computed
	if (logical)
		return extents;

	Value *vh, *vw;
	if (Shape::MixedHeightWidth (&vh, &vw)) {
		SetShapeFlags (UIElement::SHAPE_EMPTY);
		return Rect ();
	}

	Rect rect = Rect (0, 0, GetWidth (), GetHeight ());

	if ((vw && (rect.w <= 0.0)) || (vh && (rect.h <= 0.0))) { 
		SetShapeFlags (UIElement::SHAPE_EMPTY);
		return Rect ();
	}

	double t = IsStroked () ? GetStrokeThickness () : 0.0;
	switch (GetStretch ()) {
	case StretchNone:
		rect.w = rect.h = 0.0;
		break;
	case StretchUniform:
		rect.w = rect.h = MIN (rect.w, rect.h);
		break;
	case StretchUniformToFill:
		// this gets an rectangle larger than it's dimension, relative
		// scaling is ok but we need Shape::Draw to clip to it's original size
		rect.w = rect.h = MAX (rect.w, rect.h);
		break;
	case StretchFill:
		/* nothing needed here.  the assignment of w/h above
		   is correct for this case. */
		break;
	}
	
	if (rect.w == 0)
		rect.x = t *.5;
	if (rect.h == 0)
		rect.y = t *.5;

	if (t >= rect.w || t >= rect.h) {
		SetShapeFlags (UIElement::SHAPE_DEGENERATE);
		rect = rect.GrowBy (t * .5, t * .5);
	} else {
		SetShapeFlags (UIElement::SHAPE_NORMAL);
	}

	return rect;
}

// The Rectangle shape can be drawn while ignoring properties:
// * Shape::StrokeStartLineCap
// * Shape::StrokeEndLineCap
// * Shape::StrokeLineJoin	[for rounded-corner rectangles only]
// * Shape::StrokeMiterLimit	[for rounded-corner rectangles only]
bool
Rectangle::DrawShape (cairo_t *cr, bool do_op)
{
	bool drawn = Fill (cr, do_op);

	if (!stroke)
		return drawn;

	if (!SetupLine (cr))
		return drawn;

	SetupLineCaps (cr);

	if (!HasRadii ())
		SetupLineJoinMiter (cr);

	// Draw if the path wasn't drawn by the Fill call
	if (!drawn)
		Draw (cr);
	Stroke (cr, do_op);
	return true; 
}

/*
 * rendering notes:
 * - a Width="0" or a Height="0" can be rendered differently from not specifying Width or Height
 * - if a rectangle has only a Width or only a Height it is NEVER rendered
 */
void
Rectangle::BuildPath ()
{
	Value *height, *width;
	if (Shape::MixedHeightWidth (&height, &width))
		return;

	Stretch stretch = GetStretch ();
	double t = IsStroked () ? GetStrokeThickness () : 0.0;
	
	// nothing is drawn (nor filled) if no StrokeThickness="0"
	// unless both Width and Height are specified or when no streching is required
	double radius_x = 0.0, radius_y = 0.0;
	Rect rect = Rect (0, 0, GetWidth (), GetHeight ());
	GetRadius (&radius_x, &radius_y);

	switch (stretch) {
	case StretchNone:
		rect.w = rect.h = 0;
		break;
	case StretchUniform:
		rect.w = rect.h = MIN (rect.w, rect.h);
		break;
	case StretchUniformToFill:
		// this gets an rectangle larger than it's dimension, relative
		// scaling is ok but we need Shape::Draw to clip to it's original size
		rect.w = rect.h = MAX (rect.w, rect.h);
		break;
	case StretchFill:
		/* nothing needed here.  the assignment of w/h above
		   is correct for this case. */
		break;
	}
	
	if (rect.w == 0)
		rect.x = t *.5;
	if (rect.h == 0)
		rect.y = t *.5;

	if (t >= rect.w || t >= rect.h) {
		rect = rect.GrowBy (t * 0.001, t * 0.001);
		SetShapeFlags (UIElement::SHAPE_DEGENERATE);
	} else {
		rect = rect.GrowBy (-t * 0.5, -t * 0.5);
		SetShapeFlags (UIElement::SHAPE_NORMAL);
	}

	path = moon_path_renew (path, MOON_PATH_ROUNDED_RECTANGLE_LENGTH);
	moon_rounded_rectangle (path, rect.x, rect.y, rect.w, rect.h, radius_x, radius_y);
}

void
Rectangle::OnPropertyChanged (PropertyChangedEventArgs *args)
{
	if (args->property->GetOwnerType() != Type::RECTANGLE) {
		Shape::OnPropertyChanged (args);
		return;
	}

	if ((args->property == Rectangle::RadiusXProperty) || (args->property == Rectangle::RadiusYProperty)) {
		InvalidatePathCache ();
		// note: changing the X and/or Y radius doesn't affect the bounds
	}

	Invalidate ();
	NotifyListenersOfPropertyChange (args);
}

void
Rectangle::GetSizeForBrush (cairo_t *cr, double *width, double *height)
{
	switch (GetStretch ()) {
	case StretchUniform:
		*width = *height = (extents.w < extents.h) ? extents.w : extents.h;
		break;
	case StretchUniformToFill:
		*width = *height = (extents.w > extents.h) ? extents.w : extents.h;
		break;
	default:
		return Shape::GetSizeForBrush (cr, width, height);
	}
}

bool
Rectangle::GetRadius (double *rx, double *ry)
{
	Value *value = GetValueNoDefault (Rectangle::RadiusXProperty);
	if (!value)
		return false;
	*rx = value->AsDouble ();

	value = GetValueNoDefault (Rectangle::RadiusYProperty);
	if (!value)
		return false;
	*ry = value->AsDouble ();

	return ((*rx != 0.0) && (*ry != 0.0));
}

Rect
Rectangle::ComputeLargestRectangle ()
{
	double x = GetStrokeThickness ();
	double y = x;
	if (HasRadii ()) {
		x += GetRadiusX ();
		y += GetRadiusY ();
	}
	return ComputeShapeBounds (false).GrowBy (-x, -y).RoundIn ();
}

void
Rectangle::SetRadiusX (double radius)
{
	SetValue (Rectangle::RadiusXProperty, Value (radius));
}

double
Rectangle::GetRadiusX ()
{
	return GetValue (Rectangle::RadiusXProperty)->AsDouble ();
}

void
Rectangle::SetRadiusY (double radius)
{
	SetValue (Rectangle::RadiusYProperty, Value (radius));
}

double
Rectangle::GetRadiusY ()
{
	return GetValue (Rectangle::RadiusYProperty)->AsDouble ();
}


double
rectangle_get_radius_x (Rectangle *rectangle)
{
	return rectangle->GetRadiusX ();
}

void
rectangle_set_radius_x (Rectangle *rectangle, double radius)
{
	rectangle->SetRadiusX (radius);
}

double
rectangle_get_radius_y (Rectangle *rectangle)
{
	return rectangle->GetRadiusY ();
}

void
rectangle_set_radius_y (Rectangle *rectangle, double radius)
{
	rectangle->SetRadiusY (radius);
}

//
// Line
//
// rules
// * the internal path must be rebuilt when
//	- Line::X1Property, Line::Y1Property, Line::X2Property or Line::Y2Property is changed
//
// * bounds calculation is based on
//	- Line::X1Property, Line::Y1Property, Line::X2Property and Line::Y2Property
//	- Shape::StrokeThickness
//

DependencyProperty *Line::X1Property;
DependencyProperty *Line::Y1Property;
DependencyProperty *Line::X2Property;
DependencyProperty *Line::Y2Property;

#define LINECAP_SMALL_OFFSET	0.1

//Draw the start cap. Shared with Polyline
static void
line_draw_cap (cairo_t *cr, Shape* shape, PenLineCap cap, double x1, double y1, double x2, double y2)
{
	double sx1, sy1;
	if (cap == PenLineCapFlat)
		return;

	if (cap == PenLineCapRound) {
		cairo_set_line_cap (cr, convert_line_cap (cap));
		cairo_move_to (cr, x1, y1);
		cairo_line_to (cr, x1, y1);
		shape->Stroke (cr, true);
		return;
	}

	if (x1 == x2) {
		// vertical line
		sx1 = x1;
		if (y1 > y2)
			sy1 = y1 + LINECAP_SMALL_OFFSET;
		else
			sy1 = y1 - LINECAP_SMALL_OFFSET;
	} else if (y1 == y2) {
		// horizontal line
		sy1 = y1;
		if (x1 > x2)
			sx1 = x1 + LINECAP_SMALL_OFFSET;
		else
			sx1 = x1 - LINECAP_SMALL_OFFSET;
	} else {
		double m = (y1 - y2) / (x1 - x2);
		if (x1 > x2) {
			sx1 = x1 + LINECAP_SMALL_OFFSET;
		} else {
			sx1 = x1 - LINECAP_SMALL_OFFSET;
		}
		sy1 = m * sx1 + y1 - (m * x1);
	}
	cairo_set_line_cap (cr, convert_line_cap (cap));
	cairo_move_to (cr, x1, y1);
	cairo_line_to (cr, sx1, sy1);
	shape->Stroke (cr, true);
}

// The Line shape can be drawn while ignoring properties:
// * Shape::StrokeLineJoin
// * Shape::StrokeMiterLimit
// * Shape::Fill
bool
Line::DrawShape (cairo_t *cr, bool do_op)
{
	// no need to clear path since none has been drawn to cairo
	if (!stroke)
		return false; 

	if (!SetupLine (cr))
		return false;

	// here we hack around #345888 where Cairo doesn't support different start and end linecaps
	PenLineCap start = GetStrokeStartLineCap ();
	PenLineCap end = GetStrokeEndLineCap ();
	PenLineCap dash = GetStrokeDashCap ();
	bool dashed = false;
	int count = 0;
	double *dashes = GetStrokeDashArray (&count);
	
	if (dashes && (count > 0))
		dashed = true;

	//if (do_op && !(start == end && start == dash)) {
	if (do_op && (start != end || (dashed && !(start == end && start == dash)))) {
		double x1 = GetX1 ();
		double y1 = GetY1 ();
		double x2 = GetX2 ();
		double y2 = GetY2 ();
		
		// draw start and end line caps
		if (start != PenLineCapFlat) 
			line_draw_cap (cr, this, start, x1, y1, x2, y2);
		
		if (end != PenLineCapFlat) {
			//don't draw the end cap if it's in an "off" segment
			double thickness = GetStrokeThickness ();
			double offset = GetStrokeDashOffset ();
			
			SetupDashes (cr, thickness, sqrt ((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1)) + offset * thickness);
			line_draw_cap (cr, this, end, x2, y2, x1, y1);
			SetupLine (cr);
		}

		cairo_set_line_cap (cr, convert_line_cap (dash));
	} else 
		cairo_set_line_cap (cr, convert_line_cap (start));

	Draw (cr);
	Stroke (cr, do_op);
	return true;
}

void
calc_line_bounds (double x1, double x2, double y1, double y2, double thickness, PenLineCap start_cap, PenLineCap end_cap, Rect* bounds)
{
	if (x1 == x2) {
		bounds->x = x1 - thickness / 2.0;
		bounds->y = MIN (y1, y2) - (y1 < y2 && start_cap != PenLineCapFlat ? thickness / 2.0 : 0.0) - (y1 >= y2 && end_cap != PenLineCapFlat ? thickness / 2.0 : 0.0);
		bounds->w = thickness;
		bounds->h = fabs (y2 - y1) + (start_cap != PenLineCapFlat ? thickness / 2.0 : 0.0) + (end_cap != PenLineCapFlat ? thickness / 2.0 : 0.0);
	} else 	if (y1 == y2) {
		bounds->x = MIN (x1, x2) - (x1 < x2 && start_cap != PenLineCapFlat ? thickness / 2.0 : 0.0) - (x1 >= x2 && end_cap != PenLineCapFlat ? thickness / 2.0 : 0.0);
		bounds->y = y1 - thickness / 2.0;
		bounds->w = fabs (x2 - x1) + (start_cap != PenLineCapFlat ? thickness / 2.0 : 0.0) + (end_cap != PenLineCapFlat ? thickness / 2.0 : 0.0);
		bounds->h = thickness;
	} else {
		double m = fabs ((y1 - y2) / (x1 - x2));
#if EXACT_BOUNDS
		double dx = sin (atan (m)) * thickness;
		double dy = cos (atan (m)) * thickness;
#else
		double dx = (m > 1.0) ? thickness : thickness * m;
		double dy = (m < 1.0) ? thickness : thickness / m;
#endif
		if (x1 < x2)
			switch (start_cap) {
			case PenLineCapSquare:
				bounds->x = MIN (x1, x2) - (dx + dy) / 2.0;
				break;
			case PenLineCapTriangle: //FIXME, reverting to Round for now
			case PenLineCapRound:
				bounds->x = MIN (x1, x2) - thickness / 2.0;
				break;
			default: //PenLineCapFlat
				bounds->x = MIN (x1, x2) - dx / 2.0;
			}	
		else 
			switch (end_cap) {
			case PenLineCapSquare:
				bounds->x = MIN (x1, x2) - (dx + dy) / 2.0;
				break;
			case PenLineCapTriangle: //FIXME, reverting to Round for now
			case PenLineCapRound:
				bounds->x = MIN (x1, x2) - thickness / 2.0;
				break;
			default: //PenLineCapFlat
				bounds->x = MIN (x1, x2) - dx / 2.0;
			}		
		if (y1 < y2)
			switch (start_cap) {
			case PenLineCapSquare:
				bounds->y = MIN (y1, y2) - (dx + dy) / 2.0;
				break;
			case PenLineCapTriangle: //FIXME, reverting to Round for now
			case PenLineCapRound:
				bounds->y = MIN (y1, y2) - thickness / 2.0;
				break;
			default: //PenLineCapFlat
				bounds->y = MIN (y1, y2) - dy / 2.0;
			}	
		else
			switch (end_cap) {
			case PenLineCapSquare:
				bounds->y = MIN (y1, y2) - (dx + dy) / 2.0;
				break;
			case PenLineCapTriangle: //FIXME, reverting to Round for now
			case PenLineCapRound:
				bounds->y = MIN (y1, y2) - thickness / 2.0;
				break;
			default: //PenLineCapFlat
				bounds->y = MIN (y1, y2) - dy / 2.0;
			}	
		bounds->w = fabs (x2 - x1);
		bounds->h = fabs (y2 - y1);
		switch (start_cap) {
		case PenLineCapSquare:
			bounds->w += (dx + dy) / 2.0;
			bounds->h += (dx + dy) / 2.0;
			break;
		case PenLineCapTriangle: //FIXME, reverting to Round for now
		case PenLineCapRound:
			bounds->w += thickness / 2.0;
			bounds->h += thickness / 2.0;
			break;
		default: //PenLineCapFlat
			bounds->w += dx/2.0;
			bounds->h += dy/2.0;
		}
		switch (end_cap) {
		case PenLineCapSquare:
			bounds->w += (dx + dy) / 2.0;
			bounds->h += (dx + dy) / 2.0;
			break;
		case PenLineCapTriangle: //FIXME, reverting to Round for now
		case PenLineCapRound:
			bounds->w += thickness / 2.0;
			bounds->h += thickness / 2.0;
			break;
		default: //PenLineCapFlat
			bounds->w += dx/2.0;
			bounds->h += dy/2.0;	
		}
	}
}

void
Line::BuildPath ()
{
	if (Shape::MixedHeightWidth (NULL, NULL))
		return;

	SetShapeFlags (UIElement::SHAPE_NORMAL);

	path = moon_path_renew (path, MOON_PATH_MOVE_TO_LENGTH + MOON_PATH_LINE_TO_LENGTH);
	
	double x1 = GetX1 ();
	double y1 = GetY1 ();
	double x2 = GetX2 ();
	double y2 = GetY2 ();
	
	moon_move_to (path, x1, y1);
	moon_line_to (path, x2, y2);
}

Rect
Line::ComputeShapeBounds (bool logical)
{
	Rect shape_bounds = Rect ();

	if (Shape::MixedHeightWidth (NULL, NULL))
		return shape_bounds;

	double thickness;
	if (!logical)
		thickness = GetStrokeThickness ();
	else
		thickness = 0.0;

	PenLineCap start_cap, end_cap;
	if (!logical) {
		start_cap = GetStrokeStartLineCap ();
		end_cap = GetStrokeEndLineCap ();
	} else 
		start_cap = end_cap = PenLineCapFlat;
	
	if (thickness <= 0.0 && !logical)
		return shape_bounds;
	
	double x1 = GetX1 ();
	double y1 = GetY1 ();
	double x2 = GetX2 ();
	double y2 = GetY2 ();
	
	calc_line_bounds (x1, x2, y1, y2, thickness, start_cap, end_cap, &shape_bounds);

	return shape_bounds;
}

void
Line::OnPropertyChanged (PropertyChangedEventArgs *args)
{
	if (args->property->GetOwnerType() != Type::LINE) {
		Shape::OnPropertyChanged (args);
		return;
	}

	if (args->property == Line::X1Property ||
	    args->property == Line::X2Property ||
	    args->property == Line::Y1Property ||
	    args->property == Line::Y2Property) {
		InvalidatePathCache ();
		UpdateBounds (true);
	}

	NotifyListenersOfPropertyChange (args);
}

void
Line::SetX1 (double x1)
{
	SetValue (Line::X1Property, Value (x1));
}

double
Line::GetX1 ()
{
	return GetValue (Line::X1Property)->AsDouble ();
}

void
Line::SetY1 (double y1)
{
	SetValue (Line::Y1Property, Value (y1));
}

double
Line::GetY1 ()
{
	return GetValue (Line::Y1Property)->AsDouble ();
}

void
Line::SetX2 (double x2)
{
	SetValue (Line::X2Property, Value (x2));
}

double
Line::GetX2 ()
{
	return GetValue (Line::X2Property)->AsDouble ();
}

void
Line::SetY2 (double y2)
{
	SetValue (Line::Y2Property, Value (y2));
}

double
Line::GetY2 ()
{
	return GetValue (Line::Y2Property)->AsDouble ();
}


double
line_get_x1 (Line *line)
{
	return line->GetX1 ();
}

void
line_set_x1 (Line *line, double x1)
{
	line->SetX1 (x1);
}

double
line_get_y1 (Line *line)
{
	return line->GetY1 ();
}

void
line_set_y1 (Line *line, double y1)
{
	line->SetY1 (y1);
}

double
line_get_x2 (Line *line)
{
	return line->GetX2 ();
}

void
line_set_x2 (Line *line, double x2)
{
	line->SetX2 (x2);
}

double
line_get_y2 (Line *line)
{
	return line->GetY2 ();
}

void
line_set_y2 (Line *line, double y2)
{
	line->SetY2 (y2);
}



//
// Polygon
//
// rules
// * the internal path must be rebuilt when
//	- Polygon::PointsProperty is changed
//	- Shape::StretchProperty is changed
//
// * bounds calculation is based on
//	- Polygon::PointsProperty
//	- Shape::StretchProperty
//	- Shape::StrokeThickness
//

DependencyProperty *Polygon::FillRuleProperty;
DependencyProperty *Polygon::PointsProperty;


// The Polygon shape can be drawn while ignoring properties:
// * Shape::StrokeStartLineCap
// * Shape::StrokeEndLineCap
bool
Polygon::DrawShape (cairo_t *cr, bool do_op)
{
	bool drawn = Fill (cr, do_op);

	if (!stroke)
		return drawn; 

	if (!SetupLine (cr))
		return drawn;
	SetupLineCaps (cr);
	SetupLineJoinMiter (cr);

	Draw (cr);
	Stroke (cr, do_op);
	return true;
}

// special case when a polygon has a single line in it (it's drawn longer than it should)
// e.g. <Polygon Fill="#000000" Stroke="#FF00FF" StrokeThickness="8" Points="260,80 300,40" />
static void
polygon_extend_line (double *x1, double *x2, double *y1, double *y2, double thickness)
{
	// not sure why it's a 5 ? afaik it's not related to the line length or any other property
	double t5 = thickness * 5.0;
	double dx = *x1 - *x2;
	double dy = *y1 - *y2;

	if (dy == 0.0) {
		t5 -= thickness / 2.0;
		if (dx > 0.0) {
			*x1 += t5;
			*x2 -= t5;
		} else {
			*x1 -= t5;
			*x2 += t5;
		}
	} else if (dx == 0.0) {
		t5 -= thickness / 2.0;
		if (dy > 0.0) {
			*y1 += t5;
			*y2 -= t5;
		} else {
			*y1 -= t5;
			*y2 += t5;
		}
	} else {
		double angle = atan (dy / dx);
		double ax = fabs (sin (angle) * t5);
		if (dx > 0.0) {
			*x1 += ax;
			*x2 -= ax;
		} else {
			*x1 -= ax;
			*x2 += ax;
		}
		double ay = fabs (sin ((M_PI / 2.0) - angle)) * t5;
		if (dy > 0.0) {
			*y1 += ay;
			*y2 -= ay;
		} else {
			*y1 -= ay;
			*y2 += ay;
		}
	}
}

void
Polygon::BuildPath ()
{
	if (Shape::MixedHeightWidth (NULL, NULL))
		return;

	int i, count = 0;
	Point *points = GetPoints (&count);
	
	// the first point is a move to, resulting in an empty shape
	if (!points || (count < 2)) {
		SetShapeFlags (UIElement::SHAPE_EMPTY);
		return;
	}

	SetShapeFlags (UIElement::SHAPE_NORMAL);

	// 2 data per [move|line]_to + 1 for close path
	path = moon_path_renew (path, count * 2 + 1);

	// special case, both the starting and ending points are 5 * thickness than the actual points
	if (count == 2) {
		double thickness = GetStrokeThickness ();
		double x1 = points [0].x;
		double y1 = points [0].y;
		double x2 = points [1].x;
		double y2 = points [1].y;
		
		polygon_extend_line (&x1, &x2, &y1, &y2, thickness);

		moon_move_to (path, x1, y1);
		moon_line_to (path, x2, y2);
	} else {
		moon_move_to (path, points [0].x, points [0].y);
		for (i = 1; i < count; i++)
			moon_line_to (path, points [i].x, points [i].y);
	}
	moon_close_path (path);
}

void
Polygon::OnPropertyChanged (PropertyChangedEventArgs *args)
{
	if (args->property->GetOwnerType() != Type::POLYGON) {
		Shape::OnPropertyChanged (args);
		return;
	}

	if (args->property == Polygon::PointsProperty) {
		InvalidatePathCache ();
		UpdateBounds (true /* force one here, even if the bounds don't change */);
	}

	Invalidate ();
	NotifyListenersOfPropertyChange (args);
}

void
Polygon::OnCollectionChanged (Collection *col, CollectionChangedEventArgs *args)
{
	Shape::OnCollectionChanged (col, args);
	
	UpdateBounds (true);
	Invalidate ();
}

void
Polygon::OnCollectionItemChanged (Collection *col, DependencyObject *obj, PropertyChangedEventArgs *args)
{
	Shape::OnCollectionItemChanged (col, obj, args);
	
	UpdateBounds (true);
	Invalidate ();
}

void
Polygon::SetFillRule (FillRule rule)
{
	SetValue (Polygon::FillRuleProperty, Value (rule));
}

FillRule
Polygon::GetFillRule ()
{
	return (FillRule) GetValue (Polygon::FillRuleProperty)->AsInt32 ();
}

void
Polygon::SetPoints (Point *points, int n)
{
	SetValue (Polygon::PointsProperty, Value (points, n));
}

/*
 * note: We return a reference, not a copy, of the points. Not a big issue as
 * Silverlight Polygon.Points only has a setter (no getter), so it's use is 
 * only internal.
 */
Point *
Polygon::GetPoints (int *n)
{
	Value *value = GetValue (Polygon::PointsProperty);
	
	if (!value) {
		*n = 0;
		return NULL;
	}
	
	PointArray *array = value->AsPointArray();
	*n = array->basic.count;
	
	return array->points;
}

FillRule
polygon_get_fill_rule (Polygon *polygon)
{
	return polygon->GetFillRule ();
}

void
polygon_set_fill_rule (Polygon *polygon, FillRule rule)
{
	polygon->SetFillRule (rule);
}

void
polygon_set_points (Polygon *polygon, Point *points, int n)
{
	polygon->SetPoints (points, n);
}



//
// Polyline
//
// rules
// * the internal path must be rebuilt when
//	- Polyline::PointsProperty is changed
//	- Shape::StretchProperty is changed
//
// * bounds calculation is based on
//	- Polyline::PointsProperty
//	- Shape::StretchProperty
//	- Shape::StrokeThickness

DependencyProperty *Polyline::FillRuleProperty;
DependencyProperty *Polyline::PointsProperty;

// The Polyline shape can be drawn while ignoring NO properties
bool
Polyline::DrawShape (cairo_t *cr, bool do_op)
{
	bool drawn = Fill (cr, do_op);

	if (!stroke)
		return drawn; 

	if (!SetupLine (cr))
		return drawn;
	SetupLineJoinMiter (cr);

	// here we hack around #345888 where Cairo doesn't support different start and end linecaps
	PenLineCap start = GetStrokeStartLineCap ();
	PenLineCap end = GetStrokeEndLineCap ();
	PenLineCap dash = GetStrokeDashCap ();
	
	if (do_op && ! (start == end && start == dash)){
		// the previous fill, if needed, has preserved the path
		if (drawn)
			cairo_new_path (cr);

		// since Draw may not have been called (e.g. no Fill) we must ensure the path was built
		if (!drawn || !path || (path->cairo.num_data == 0))
			BuildPath ();

		cairo_path_data_t *data = path->cairo.data;
		int length = path->cairo.num_data;
		// single point polylines are not rendered
		if (length >= MOON_PATH_MOVE_TO_LENGTH + MOON_PATH_LINE_TO_LENGTH) {
			// draw line #1 with start cap
			if (start != PenLineCapFlat) {
				line_draw_cap (cr, this, start, data[1].point.x, data[1].point.y, data[3].point.x, data[3].point.y);
			}
			// draw last line with end cap
			if (end != PenLineCapFlat) {
				line_draw_cap (cr, this, end, data[length-1].point.x, data[length-1].point.y, data[length-3].point.x, data[length-3].point.y);
			}
		}
	}
	cairo_set_line_cap (cr, convert_line_cap (dash));

	Draw (cr);
	Stroke (cr, do_op);
	return true;
}

void
Polyline::BuildPath ()
{
	if (Shape::MixedHeightWidth (NULL, NULL))
		return;

	int i, count = 0;
	Point *points = GetPoints (&count);
	
	// the first point is a move to, resulting in an empty shape
	if (!points || (count < 2)) {
		SetShapeFlags (UIElement::SHAPE_EMPTY);
		return;
	}

	SetShapeFlags (UIElement::SHAPE_NORMAL);

	// 2 data per [move|line]_to
	path = moon_path_renew (path, count * 2);

	moon_move_to (path, points [0].x, points [0].y);
	for (i = 1; i < count; i++)
		moon_line_to (path, points [i].x, points [i].y);
}

void
Polyline::OnPropertyChanged (PropertyChangedEventArgs *args)
{
	if (args->property->GetOwnerType() != Type::POLYLINE) {
		Shape::OnPropertyChanged (args);
		return;
	}

	if (args->property == Polyline::PointsProperty) {
		InvalidatePathCache ();
		UpdateBounds (true /* force one here, even if the bounds don't change */);
	}

	Invalidate ();
	NotifyListenersOfPropertyChange (args);
}

void
Polyline::OnCollectionChanged (Collection *col, CollectionChangedEventArgs *args)
{
	Shape::OnCollectionChanged (col, args);
	
	UpdateBounds ();
	Invalidate ();
}

void
Polyline::OnCollectionItemChanged (Collection *col, DependencyObject *obj, PropertyChangedEventArgs *args)
{
	Shape::OnCollectionItemChanged (col, obj, args);
	
	UpdateBounds ();
	Invalidate ();
}

void
Polyline::SetFillRule (FillRule rule)
{
	SetValue (Polyline::FillRuleProperty, Value (rule));
}

FillRule
Polyline::GetFillRule ()
{
	return (FillRule) GetValue (Polyline::FillRuleProperty)->AsInt32 ();
}

void
Polyline::SetPoints (Point *points, int n)
{
	SetValue (Polyline::PointsProperty, Value (points, n));
}

/*
 * note: We return a reference, not a copy, of the points. Not a big issue as
 * Silverlight Polyline.Points only has a setter (no getter), so it's use is 
 * only internal.
 */
Point *
Polyline::GetPoints (int *n)
{
	Value *value = GetValue (Polyline::PointsProperty);
	
	if (!value) {
		*n = 0;
		return NULL;
	}
	
	PointArray *array = value->AsPointArray();
	*n = array->basic.count;
	
	return array->points;
}

FillRule
polyline_get_fill_rule (Polyline *polyline)
{
	return polyline->GetFillRule ();
}

void
polyline_set_fill_rule (Polyline *polyline, FillRule rule)
{
	polyline->SetFillRule (rule);
}

void
polyline_set_points (Polyline *polyline, Point *points, int n)
{
	polyline->SetPoints (points, n);
}


//
// Path
//

DependencyProperty *Path::DataProperty;

bool
Path::SetupLine (cairo_t* cr)
{
	// we cannot use the thickness==0 optimization (like Shape::SetupLine provides)
	// since we'll be using cairo to compute the path's bounds later
	// see bug #352188 for an example of what this breaks
	double thickness = IsDegenerate () ? 1.0 : GetStrokeThickness ();
	cairo_set_line_width (cr, thickness);
	return SetupDashes (cr, thickness);
}

// The Polygon shape can be drawn while ignoring properties:
// * none 
// FIXME: actually it depends on the geometry, another level of optimization awaits ;-)
// e.g. close geometries don't need to setup line caps, 
//	line join/miter	don't applies to curve, like EllipseGeometry
bool
Path::DrawShape (cairo_t *cr, bool do_op)
{
	bool drawn = Shape::Fill (cr, do_op);

	if (stroke) {
		if (!SetupLine (cr))
			return drawn;	// return if we have a path in the cairo_t
		SetupLineCaps (cr);
		SetupLineJoinMiter (cr);

		if (!drawn)
			Draw (cr);
		Stroke (cr, do_op);
	}

	return true;
}

FillRule
Path::GetFillRule ()
{
	Geometry *geometry;
	
	if (!(geometry = GetData ()))
		return Shape::GetFillRule ();
	
	return geometry->GetFillRule ();
}

Rect
Path::ComputeShapeBounds (bool logical, cairo_matrix_t *matrix)
{
	Rect shape_bounds = Rect ();
	
	Value *vh, *vw;
	if (Shape::MixedHeightWidth (&vh, &vw))
		return shape_bounds;
	
	Geometry *geometry;
	
	if (!(geometry = GetData ())) {
		SetShapeFlags (UIElement::SHAPE_EMPTY);
		return shape_bounds;
	}
	
	double w = vw ? vw->AsDouble () : 0.0;
	double h = vh ? vh->AsDouble () : 0.0;

	if ((h < 0.0) || (w < 0.0)) {
		SetShapeFlags (UIElement::SHAPE_EMPTY);
		return shape_bounds;
	}

	if ((vh && (h <= 0.0)) || (vw && (w <= 0.0))) { 
		SetShapeFlags (UIElement::SHAPE_EMPTY);
		return shape_bounds;
	}

	shape_bounds = geometry->ComputeBounds (this, logical, matrix);

	return shape_bounds;
}

void
Path::Draw (cairo_t *cr)
{
	cairo_new_path (cr);
	
	Geometry *geometry;
	
	if (!(geometry = GetData ()))
		return;
	
	cairo_save (cr);
	cairo_transform (cr, &stretch_transform);
	geometry->Draw (this, cr);
	cairo_restore (cr);
}

void
Path::OnPropertyChanged (PropertyChangedEventArgs *args)
{
	if (args->property->GetOwnerType() != Type::PATH) {
		Shape::OnPropertyChanged (args);
		return;
	}

	InvalidatePathCache ();
	FullInvalidate (false);

	NotifyListenersOfPropertyChange (args);
}

void
Path::OnSubPropertyChanged (DependencyProperty *prop, DependencyObject *obj, PropertyChangedEventArgs *subobj_args)
{
	if (prop == Path::DataProperty) {
		InvalidatePathCache ();
		FullInvalidate (false);
	}
	else
		Shape::OnSubPropertyChanged (prop, obj, subobj_args);
}

/*
 * Right now implementing Path::ComputeLargestRectangle doesn't seems like a good idea. That would require
 * - checking the path for curves (and either flatten it or return an empty Rect)
 * - checking for polygon simplicity (finding intersections)
 * - checking for a convex polygon (if concave we can turn it into several convex or return an empty Rect)
 * - find the largest rectangle inside the (or each) convex polygon(s)
 * 	http://cgm.cs.mcgill.ca/~athens/cs507/Projects/2003/DanielSud/complete.html
 */

void
Path::SetData (Geometry *data)
{
	SetValue (Path::DataProperty, Value (data));
}

Geometry *
Path::GetData ()
{
	Value *value = GetValue (Path::DataProperty);
	
	return value ? value->AsGeometry () : NULL;
}


Geometry *
path_get_data (Path *path)
{
	return path->GetData ();
}

void
path_set_data (Path *path, Geometry *data)
{
	path->SetData (data);
}


//
// 
//

void
shape_init (void)
{
	/* Shape fields */
	Shape::FillProperty = DependencyProperty::Register (Type::SHAPE, "Fill", Type::BRUSH);
	Shape::StretchProperty = DependencyProperty::Register (Type::SHAPE, "Stretch", new Value (StretchNone));
	Shape::StrokeProperty = DependencyProperty::Register (Type::SHAPE, "Stroke", Type::BRUSH);
	Shape::StrokeDashArrayProperty = DependencyProperty::Register (Type::SHAPE, "StrokeDashArray", Type::DOUBLE_ARRAY);
	Shape::StrokeDashCapProperty = DependencyProperty::Register (Type::SHAPE, "StrokeDashCap", new Value (PenLineCapFlat));
	Shape::StrokeDashOffsetProperty = DependencyProperty::Register (Type::SHAPE, "StrokeDashOffset", new Value (0.0));
	Shape::StrokeEndLineCapProperty = DependencyProperty::Register (Type::SHAPE, "StrokeEndLineCap", new Value (PenLineCapFlat));
	Shape::StrokeLineJoinProperty = DependencyProperty::Register (Type::SHAPE, "StrokeLineJoin", new Value (PenLineJoinMiter));
	Shape::StrokeMiterLimitProperty = DependencyProperty::Register (Type::SHAPE, "StrokeMiterLimit", new Value (10.0));
	Shape::StrokeStartLineCapProperty = DependencyProperty::Register (Type::SHAPE, "StrokeStartLineCap", new Value (PenLineCapFlat));
	Shape::StrokeThicknessProperty = DependencyProperty::Register (Type::SHAPE, "StrokeThickness", new Value (1.0));

	/* Rectangle fields */
	Rectangle::RadiusXProperty = DependencyProperty::Register (Type::RECTANGLE, "RadiusX", new Value (0.0));
	Rectangle::RadiusYProperty = DependencyProperty::Register (Type::RECTANGLE, "RadiusY", new Value (0.0));

	/* Line fields */
	Line::X1Property = DependencyProperty::Register (Type::LINE, "X1", new Value (0.0));
	Line::Y1Property = DependencyProperty::Register (Type::LINE, "Y1", new Value (0.0));
	Line::X2Property = DependencyProperty::Register (Type::LINE, "X2", new Value (0.0));
	Line::Y2Property = DependencyProperty::Register (Type::LINE, "Y2", new Value (0.0));

	/* Polygon fields */
	Polygon::FillRuleProperty = DependencyProperty::Register (Type::POLYGON, "FillRule", new Value (FillRuleEvenOdd));
	Polygon::PointsProperty = DependencyProperty::Register (Type::POLYGON, "Points", Type::POINT_ARRAY);

	/* Polyline fields */
	Polyline::FillRuleProperty = DependencyProperty::Register (Type::POLYLINE, "FillRule", new Value (FillRuleEvenOdd));
	Polyline::PointsProperty = DependencyProperty::Register (Type::POLYLINE, "Points", Type::POINT_ARRAY);

	/* Path fields */
	Path::DataProperty = DependencyProperty::Register (Type::PATH, "Data", Type::GEOMETRY);
}
