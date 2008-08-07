/* 
	this file was autogenerated. do not edit this file 
 */

#include "config.h"

#include <stdlib.h>
#include <stdio.h>

#include "cbinding.h"

/* 
 * Application
 */ 

#if SL_2_0
Application*
application_new ()
{
	return new Application ();
}
#endif

/* 
 * ArcSegment
 */ 

ArcSegment*
arc_segment_new ()
{
	return new ArcSegment ();
}

/* 
 * AssemblyPart
 */ 

#if SL_2_0
AssemblyPart*
assembly_part_new ()
{
	return new AssemblyPart ();
}
#endif

/* 
 * AssemblyPartCollection
 */ 

#if SL_2_0
AssemblyPartCollection*
assembly_part_collection_new ()
{
	return new AssemblyPartCollection ();
}
#endif

/* 
 * BeginStoryboard
 */ 

BeginStoryboard*
begin_storyboard_new ()
{
	return new BeginStoryboard ();
}

/* 
 * BezierSegment
 */ 

BezierSegment*
bezier_segment_new ()
{
	return new BezierSegment ();
}

/* 
 * Brush
 */ 

Brush*
brush_new ()
{
	return new Brush ();
}

/* 
 * Canvas
 */ 

Canvas*
canvas_new ()
{
	return new Canvas ();
}

/* 
 * ColorAnimation
 */ 

ColorAnimation*
color_animation_new ()
{
	return new ColorAnimation ();
}

/* 
 * ColorAnimationUsingKeyFrames
 */ 

ColorAnimationUsingKeyFrames*
color_animation_using_key_frames_new ()
{
	return new ColorAnimationUsingKeyFrames ();
}

/* 
 * ColorKeyFrame
 */ 

ColorKeyFrame*
color_key_frame_new ()
{
	return new ColorKeyFrame ();
}

/* 
 * ColorKeyFrameCollection
 */ 

ColorKeyFrameCollection*
color_key_frame_collection_new ()
{
	return new ColorKeyFrameCollection ();
}

/* 
 * ColumnDefinition
 */ 

#if SL_2_0
ColumnDefinition*
column_definition_new ()
{
	return new ColumnDefinition ();
}
#endif

/* 
 * ColumnDefinitionCollection
 */ 

#if SL_2_0
ColumnDefinitionCollection*
column_definition_collection_new ()
{
	return new ColumnDefinitionCollection ();
}
#endif

/* 
 * ContentControl
 */ 

#if SL_2_0
ContentControl*
content_control_new ()
{
	return new ContentControl ();
}
#endif

/* 
 * Control
 */ 

#if SL_2_0
Control*
control_new ()
{
	return new Control ();
}
#endif

/* 
 * DependencyObject
 */ 

#if SL_2_0
Value*
dependency_object_get_default_value_with_error (DependencyObject* instance, Types* additional_types, DependencyProperty* property, MoonError* error)
{
	if (instance == NULL)
		return NULL;
	
	if (error == NULL)
		g_warning ("Moonlight: Called dependency_object_get_default_value_with_error () with error == NULL.");
	return instance->GetDefaultValueWithError (additional_types, property, error);
}
#endif

const char*
dependency_object_get_name (DependencyObject* instance)
{
	if (instance == NULL)
		return NULL;
	
	return instance->GetName ();
}

#if SL_2_0
Value*
dependency_object_get_value_no_default_with_error (DependencyObject* instance, Types* additional_types, DependencyProperty* property, MoonError* error)
{
	if (instance == NULL)
		return NULL;
	
	if (error == NULL)
		g_warning ("Moonlight: Called dependency_object_get_value_no_default_with_error () with error == NULL.");
	return instance->GetValueNoDefaultWithError (additional_types, property, error);
}
#endif

#if SL_2_0
Value*
dependency_object_get_value_with_error (DependencyObject* instance, Types* additional_types, Type::Kind whatami, DependencyProperty* property, MoonError* error)
{
	if (instance == NULL)
		return NULL;
	
	if (error == NULL)
		g_warning ("Moonlight: Called dependency_object_get_value_with_error () with error == NULL.");
	return instance->GetValueWithError (additional_types, whatami, property, error);
}
#endif

/* 
 * DependencyObjectCollection
 */ 

DependencyObjectCollection*
dependency_object_collection_new ()
{
	return new DependencyObjectCollection ();
}

/* 
 * DependencyProperty
 */ 

DependencyProperty*
dependency_property_get_dependency_property (Type::Kind type, const char* name)
{
	return DependencyProperty::GetDependencyProperty (type, name);
}

const char*
dependency_property_get_name (DependencyProperty* instance)
{
	if (instance == NULL)
		return NULL;
	
	return instance->GetName ();
}

Type::Kind
dependency_property_get_property_type (DependencyProperty* instance)
{
	if (instance == NULL)
		return Type::INVALID;
	
	return instance->GetPropertyType ();
}

bool
dependency_property_is_nullable (DependencyProperty* instance)
{
	if (instance == NULL)
		return false;;
	
	return instance->IsNullable ();
}

DependencyProperty*
dependency_property_register (Type::Kind type, const char* name, Value* default_value)
{
	return DependencyProperty::Register (type, name, default_value);
}

#if SL_2_0
DependencyProperty*
dependency_property_register_full (Types* additional_types, Type::Kind type, const char* name, Value* default_value, Type::Kind vtype, bool attached, bool read_only, bool always_change, NativePropertyChangedHandler* changed_callback)
{
	return DependencyProperty::RegisterFull (additional_types, type, name, default_value, vtype, attached, read_only, always_change, changed_callback);
}
#endif

#if SL_2_0
DependencyProperty*
dependency_property_register_managed_property (Types* additional_types, const char* name, Type::Kind property_type, Type::Kind owner_type, bool attached, NativePropertyChangedHandler* callback)
{
	return DependencyProperty::RegisterManagedProperty (additional_types, name, property_type, owner_type, attached, callback);
}
#endif

/* 
 * Deployment
 */ 

#if SL_2_0
Deployment*
deployment_new ()
{
	return new Deployment ();
}
#endif

/* 
 * DiscreteColorKeyFrame
 */ 

DiscreteColorKeyFrame*
discrete_color_key_frame_new ()
{
	return new DiscreteColorKeyFrame ();
}

/* 
 * DiscreteDoubleKeyFrame
 */ 

DiscreteDoubleKeyFrame*
discrete_double_key_frame_new ()
{
	return new DiscreteDoubleKeyFrame ();
}

/* 
 * DiscretePointKeyFrame
 */ 

DiscretePointKeyFrame*
discrete_point_key_frame_new ()
{
	return new DiscretePointKeyFrame ();
}

/* 
 * DoubleAnimation
 */ 

DoubleAnimation*
double_animation_new ()
{
	return new DoubleAnimation ();
}

/* 
 * DoubleAnimationUsingKeyFrames
 */ 

DoubleAnimationUsingKeyFrames*
double_animation_using_key_frames_new ()
{
	return new DoubleAnimationUsingKeyFrames ();
}

/* 
 * DoubleCollection
 */ 

DoubleCollection*
double_collection_new ()
{
	return new DoubleCollection ();
}

/* 
 * DoubleKeyFrame
 */ 

DoubleKeyFrame*
double_key_frame_new ()
{
	return new DoubleKeyFrame ();
}

/* 
 * DoubleKeyFrameCollection
 */ 

DoubleKeyFrameCollection*
double_key_frame_collection_new ()
{
	return new DoubleKeyFrameCollection ();
}

/* 
 * Downloader
 */ 

Downloader*
downloader_new ()
{
	return new Downloader ();
}

/* 
 * DrawingAttributes
 */ 

DrawingAttributes*
drawing_attributes_new ()
{
	return new DrawingAttributes ();
}

/* 
 * Ellipse
 */ 

Ellipse*
ellipse_new ()
{
	return new Ellipse ();
}

/* 
 * EllipseGeometry
 */ 

EllipseGeometry*
ellipse_geometry_new ()
{
	return new EllipseGeometry ();
}

/* 
 * EventTrigger
 */ 

EventTrigger*
event_trigger_new ()
{
	return new EventTrigger ();
}

/* 
 * FrameworkElement
 */ 

FrameworkElement*
framework_element_new ()
{
	return new FrameworkElement ();
}

/* 
 * GeneralTransform
 */ 

GeneralTransform*
general_transform_new ()
{
	return new GeneralTransform ();
}

/* 
 * GeometryCollection
 */ 

GeometryCollection*
geometry_collection_new ()
{
	return new GeometryCollection ();
}

/* 
 * GeometryGroup
 */ 

GeometryGroup*
geometry_group_new ()
{
	return new GeometryGroup ();
}

/* 
 * Glyphs
 */ 

Glyphs*
glyphs_new ()
{
	return new Glyphs ();
}

/* 
 * GradientBrush
 */ 

GradientBrush*
gradient_brush_new ()
{
	return new GradientBrush ();
}

/* 
 * GradientStop
 */ 

GradientStop*
gradient_stop_new ()
{
	return new GradientStop ();
}

/* 
 * GradientStopCollection
 */ 

GradientStopCollection*
gradient_stop_collection_new ()
{
	return new GradientStopCollection ();
}

/* 
 * Grid
 */ 

#if SL_2_0
Grid*
grid_new ()
{
	return new Grid ();
}
#endif

/* 
 * Image
 */ 

Image*
image_new ()
{
	return new Image ();
}

/* 
 * ImageBrush
 */ 

ImageBrush*
image_brush_new ()
{
	return new ImageBrush ();
}

/* 
 * InkPresenter
 */ 

InkPresenter*
ink_presenter_new ()
{
	return new InkPresenter ();
}

/* 
 * Inline
 */ 

Inline*
inline_new ()
{
	return new Inline ();
}

/* 
 * Inlines
 */ 

Inlines*
inlines_new ()
{
	return new Inlines ();
}

/* 
 * KeyFrame
 */ 

KeyFrame*
key_frame_new ()
{
	return new KeyFrame ();
}

/* 
 * KeyFrameCollection
 */ 

KeyFrameCollection*
key_frame_collection_new ()
{
	return new KeyFrameCollection ();
}

/* 
 * KeySpline
 */ 

KeySpline*
key_spline_new ()
{
	return new KeySpline ();
}

/* 
 * Line
 */ 

Line*
line_new ()
{
	return new Line ();
}

/* 
 * LinearColorKeyFrame
 */ 

LinearColorKeyFrame*
linear_color_key_frame_new ()
{
	return new LinearColorKeyFrame ();
}

/* 
 * LinearDoubleKeyFrame
 */ 

LinearDoubleKeyFrame*
linear_double_key_frame_new ()
{
	return new LinearDoubleKeyFrame ();
}

/* 
 * LinearGradientBrush
 */ 

LinearGradientBrush*
linear_gradient_brush_new ()
{
	return new LinearGradientBrush ();
}

/* 
 * LinearPointKeyFrame
 */ 

LinearPointKeyFrame*
linear_point_key_frame_new ()
{
	return new LinearPointKeyFrame ();
}

/* 
 * LineBreak
 */ 

LineBreak*
line_break_new ()
{
	return new LineBreak ();
}

/* 
 * LineGeometry
 */ 

LineGeometry*
line_geometry_new ()
{
	return new LineGeometry ();
}

/* 
 * LineSegment
 */ 

LineSegment*
line_segment_new ()
{
	return new LineSegment ();
}

/* 
 * Matrix
 */ 

Matrix*
matrix_new ()
{
	return new Matrix ();
}

/* 
 * MatrixTransform
 */ 

MatrixTransform*
matrix_transform_new ()
{
	return new MatrixTransform ();
}

/* 
 * MediaAttribute
 */ 

MediaAttribute*
media_attribute_new ()
{
	return new MediaAttribute ();
}

/* 
 * MediaAttributeCollection
 */ 

MediaAttributeCollection*
media_attribute_collection_new ()
{
	return new MediaAttributeCollection ();
}

/* 
 * MediaElement
 */ 

MediaElement*
media_element_new ()
{
	return new MediaElement ();
}

/* 
 * MouseEventArgs
 */ 

MouseEventArgs*
mouse_event_args_new ()
{
	return new MouseEventArgs ();
}

/* 
 * Panel
 */ 

Panel*
panel_new ()
{
	return new Panel ();
}

/* 
 * ParallelTimeline
 */ 

ParallelTimeline*
parallel_timeline_new ()
{
	return new ParallelTimeline ();
}

/* 
 * Path
 */ 

Path*
path_new ()
{
	return new Path ();
}

/* 
 * PathFigure
 */ 

PathFigure*
path_figure_new ()
{
	return new PathFigure ();
}

/* 
 * PathFigureCollection
 */ 

PathFigureCollection*
path_figure_collection_new ()
{
	return new PathFigureCollection ();
}

/* 
 * PathGeometry
 */ 

PathGeometry*
path_geometry_new ()
{
	return new PathGeometry ();
}

/* 
 * PathSegmentCollection
 */ 

PathSegmentCollection*
path_segment_collection_new ()
{
	return new PathSegmentCollection ();
}

/* 
 * PointAnimation
 */ 

PointAnimation*
point_animation_new ()
{
	return new PointAnimation ();
}

/* 
 * PointAnimationUsingKeyFrames
 */ 

PointAnimationUsingKeyFrames*
point_animation_using_key_frames_new ()
{
	return new PointAnimationUsingKeyFrames ();
}

/* 
 * PointCollection
 */ 

PointCollection*
point_collection_new ()
{
	return new PointCollection ();
}

/* 
 * PointKeyFrame
 */ 

PointKeyFrame*
point_key_frame_new ()
{
	return new PointKeyFrame ();
}

/* 
 * PointKeyFrameCollection
 */ 

PointKeyFrameCollection*
point_key_frame_collection_new ()
{
	return new PointKeyFrameCollection ();
}

/* 
 * PolyBezierSegment
 */ 

PolyBezierSegment*
poly_bezier_segment_new ()
{
	return new PolyBezierSegment ();
}

/* 
 * Polygon
 */ 

Polygon*
polygon_new ()
{
	return new Polygon ();
}

/* 
 * Polyline
 */ 

Polyline*
polyline_new ()
{
	return new Polyline ();
}

/* 
 * PolyLineSegment
 */ 

PolyLineSegment*
poly_line_segment_new ()
{
	return new PolyLineSegment ();
}

/* 
 * PolyQuadraticBezierSegment
 */ 

PolyQuadraticBezierSegment*
poly_quadratic_bezier_segment_new ()
{
	return new PolyQuadraticBezierSegment ();
}

/* 
 * QuadraticBezierSegment
 */ 

QuadraticBezierSegment*
quadratic_bezier_segment_new ()
{
	return new QuadraticBezierSegment ();
}

/* 
 * RadialGradientBrush
 */ 

RadialGradientBrush*
radial_gradient_brush_new ()
{
	return new RadialGradientBrush ();
}

/* 
 * Rectangle
 */ 

Rectangle*
rectangle_new ()
{
	return new Rectangle ();
}

/* 
 * RectangleGeometry
 */ 

RectangleGeometry*
rectangle_geometry_new ()
{
	return new RectangleGeometry ();
}

/* 
 * ResourceDictionary
 */ 

ResourceDictionary*
resource_dictionary_new ()
{
	return new ResourceDictionary ();
}

/* 
 * RotateTransform
 */ 

RotateTransform*
rotate_transform_new ()
{
	return new RotateTransform ();
}

/* 
 * RoutedEventArgs
 */ 

RoutedEventArgs*
routed_event_args_new ()
{
	return new RoutedEventArgs ();
}

/* 
 * RowDefinition
 */ 

#if SL_2_0
RowDefinition*
row_definition_new ()
{
	return new RowDefinition ();
}
#endif

/* 
 * RowDefinitionCollection
 */ 

#if SL_2_0
RowDefinitionCollection*
row_definition_collection_new ()
{
	return new RowDefinitionCollection ();
}
#endif

/* 
 * Run
 */ 

Run*
run_new ()
{
	return new Run ();
}

/* 
 * ScaleTransform
 */ 

ScaleTransform*
scale_transform_new ()
{
	return new ScaleTransform ();
}

/* 
 * SkewTransform
 */ 

SkewTransform*
skew_transform_new ()
{
	return new SkewTransform ();
}

/* 
 * SolidColorBrush
 */ 

SolidColorBrush*
solid_color_brush_new ()
{
	return new SolidColorBrush ();
}

/* 
 * SplineColorKeyFrame
 */ 

SplineColorKeyFrame*
spline_color_key_frame_new ()
{
	return new SplineColorKeyFrame ();
}

/* 
 * SplineDoubleKeyFrame
 */ 

SplineDoubleKeyFrame*
spline_double_key_frame_new ()
{
	return new SplineDoubleKeyFrame ();
}

/* 
 * SplinePointKeyFrame
 */ 

SplinePointKeyFrame*
spline_point_key_frame_new ()
{
	return new SplinePointKeyFrame ();
}

/* 
 * Storyboard
 */ 

Storyboard*
storyboard_new ()
{
	return new Storyboard ();
}

/* 
 * Stroke
 */ 

Stroke*
stroke_new ()
{
	return new Stroke ();
}

/* 
 * StrokeCollection
 */ 

StrokeCollection*
stroke_collection_new ()
{
	return new StrokeCollection ();
}

/* 
 * StylusInfo
 */ 

StylusInfo*
stylus_info_new ()
{
	return new StylusInfo ();
}

/* 
 * StylusPoint
 */ 

StylusPoint*
stylus_point_new ()
{
	return new StylusPoint ();
}

/* 
 * StylusPointCollection
 */ 

StylusPointCollection*
stylus_point_collection_new ()
{
	return new StylusPointCollection ();
}

/* 
 * TextBlock
 */ 

TextBlock*
text_block_new ()
{
	return new TextBlock ();
}

/* 
 * TimelineCollection
 */ 

TimelineCollection*
timeline_collection_new ()
{
	return new TimelineCollection ();
}

/* 
 * TimelineGroup
 */ 

TimelineGroup*
timeline_group_new ()
{
	return new TimelineGroup ();
}

/* 
 * TimelineMarker
 */ 

TimelineMarker*
timeline_marker_new ()
{
	return new TimelineMarker ();
}

/* 
 * TimelineMarkerCollection
 */ 

TimelineMarkerCollection*
timeline_marker_collection_new ()
{
	return new TimelineMarkerCollection ();
}

/* 
 * Transform
 */ 

Transform*
transform_new ()
{
	return new Transform ();
}

/* 
 * TransformCollection
 */ 

TransformCollection*
transform_collection_new ()
{
	return new TransformCollection ();
}

/* 
 * TransformGroup
 */ 

TransformGroup*
transform_group_new ()
{
	return new TransformGroup ();
}

/* 
 * TranslateTransform
 */ 

TranslateTransform*
translate_transform_new ()
{
	return new TranslateTransform ();
}

/* 
 * TriggerActionCollection
 */ 

TriggerActionCollection*
trigger_action_collection_new ()
{
	return new TriggerActionCollection ();
}

/* 
 * TriggerCollection
 */ 

TriggerCollection*
trigger_collection_new ()
{
	return new TriggerCollection ();
}

/* 
 * Types
 */ 

#if SL_2_0
void
types_free (Types* instance)
{
	delete instance;
}
#endif

#if SL_2_0
Type*
types_find (Types* instance, Type::Kind type)
{
	if (instance == NULL)
		return NULL;
	
	return instance->Find (type);
}
#endif

#if SL_2_0
Type::Kind
types_register_type (Types* instance, const char* name, void* gc_handle, Type::Kind parent)
{
	if (instance == NULL)
		return Type::INVALID;
	
	return instance->RegisterType (name, gc_handle, parent);
}
#endif

#if SL_2_0
Types*
types_new ()
{
	return new Types ();
}
#endif

/* 
 * UIElement
 */ 

UIElement*
uielement_new ()
{
	return new UIElement ();
}

/* 
 * UIElementCollection
 */ 

UIElementCollection*
uielement_collection_new ()
{
	return new UIElementCollection ();
}

/* 
 * UserControl
 */ 

#if SL_2_0
UserControl*
user_control_new ()
{
	return new UserControl ();
}
#endif

/* 
 * VideoBrush
 */ 

VideoBrush*
video_brush_new ()
{
	return new VideoBrush ();
}

/* 
 * VisualBrush
 */ 

VisualBrush*
visual_brush_new ()
{
	return new VisualBrush ();
}

