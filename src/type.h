/*
 * Automatically generated from type.h.in, do not edit this file directly
 * To regenerate execute typegen.sh
*/
/*
 * type.h: Generated code for the type system.
 *
 * Author:
 *   Rolf Bjarne Kvinge (RKvinge@novell.com)
 *
 * Copyright 2007 Novell, Inc. (http://www.novell.com)
 *
 * See the LICENSE file included with the distribution for details.
 * 
 */
#ifndef __TYPE_H__
#define __TYPE_H__

G_BEGIN_DECLS

#include <glib.h>

typedef gint64 TimeSpan;

class Type {
public:
	enum Kind {
	// START_MANAGED_MAPPING
		INVALID,
		BOOL,
		DOUBLE,
		UINT64,
		INT32,
		STRING,
		COLOR,
		POINT,
		RECT,
		REPEATBEHAVIOR,
		DURATION,
		INT64,
		TIMESPAN,
		DOUBLE_ARRAY,
		POINT_ARRAY,
		KEYTIME,
		MATRIX,
		NPOBJ,

		DEPENDENCY_OBJECT,
		ANIMATION,
		ANIMATIONCLOCK,
		ARCSEGMENT,
		BEGINSTORYBOARD,
		BEZIERSEGMENT,
		BRUSH,
		CANVAS,
		CLOCK,
		CLOCKGROUP,
		COLLECTION,
		COLORANIMATION,
		COLORANIMATIONUSINGKEYFRAMES,
		COLORKEYFRAME,
		COLORKEYFRAME_COLLECTION,
		CONTROL,
		DISCRETECOLORKEYFRAME,
		DISCRETEDOUBLEKEYFRAME,
		DISCRETEPOINTKEYFRAME,
		DOUBLEANIMATION,
		DOUBLEANIMATIONUSINGKEYFRAMES,
		DOUBLEKEYFRAME,
		DOUBLEKEYFRAME_COLLECTION,
		DOWNLOADER,
		DRAWINGATTRIBUTES,
		ELLIPSE,
		ELLIPSEGEOMETRY,
		EVENTTRIGGER,
		FRAMEWORKELEMENT,
		GEOMETRY,
		GEOMETRY_COLLECTION,
		GEOMETRYGROUP,
		GLYPHS,
		GRADIENTBRUSH,
		GRADIENTSTOP,
		GRADIENTSTOP_COLLECTION,
		IMAGE,
		IMAGEBRUSH,
		INKPRESENTER,
		INLINE,
		INLINES,
		KEYFRAME,
		KEYFRAME_COLLECTION,
		KEYSPLINE,
		LINE,
		LINEARCOLORKEYFRAME,
		LINEARDOUBLEKEYFRAME,
		LINEARGRADIENTBRUSH,
		LINEARPOINTKEYFRAME,
		LINEBREAK,
		LINEGEOMETRY,
		LINESEGMENT,
		MATRIXTRANSFORM,
		MEDIAATTRIBUTE,
		MEDIAATTRIBUTE_COLLECTION,
		MEDIABASE,
		MEDIAELEMENT,
		NAMESCOPE,
		PANEL,
		PARALLELTIMELINE,
		PATH,
		PATHFIGURE,
		PATHFIGURE_COLLECTION,
		PATHGEOMETRY,
		PATHSEGMENT,
		PATHSEGMENT_COLLECTION,
		POINTANIMATION,
		POINTANIMATIONUSINGKEYFRAMES,
		POINTKEYFRAME,
		POINTKEYFRAME_COLLECTION,
		POLYBEZIERSEGMENT,
		POLYGON,
		POLYLINE,
		POLYLINESEGMENT,
		POLYQUADRATICBEZIERSEGMENT,
		QUADRATICBEZIERSEGMENT,
		RADIALGRADIENTBRUSH,
		RECTANGLE,
		RECTANGLEGEOMETRY,
		RESOURCE_DICTIONARY,
		ROTATETRANSFORM,
		RUN,
		SCALETRANSFORM,
		SHAPE,
		SKEWTRANSFORM,
		SOLIDCOLORBRUSH,
		SPLINECOLORKEYFRAME,
		SPLINEDOUBLEKEYFRAME,
		SPLINEPOINTKEYFRAME,
		STORYBOARD,
		STROKE,
		STROKE_COLLECTION,
		STYLUSINFO,
		STYLUSPOINT,
		STYLUSPOINT_COLLECTION,
		TEXTBLOCK,
		TILEBRUSH,
		TIMELINE,
		TIMELINE_COLLECTION,
		TIMELINEGROUP,
		TIMELINEMARKER,
		TIMELINEMARKER_COLLECTION,
		TRANSFORM,
		TRANSFORM_COLLECTION,
		TRANSFORMGROUP,
		TRANSLATETRANSFORM,
		TRIGGERACTION,
		TRIGGERACTION_COLLECTION,
		TRIGGER_COLLECTION,
		UIELEMENT,
		VIDEOBRUSH,
		VISUAL,
		VISUALBRUSH,
		VISUAL_COLLECTION,

	
		LASTTYPE,
	// END_MANAGED_MAPPING
		};

	static Type* RegisterType (char *name, Type::Kind type, Type::Kind parent, bool value_type);
	static Type* RegisterType (char *name, Type::Kind type, Type::Kind parent);
	static Type* RegisterType (char *name, Type::Kind type, bool value_type);
	static Type* Find (char *name);
	static Type* Find (Type::Kind type);
	
	bool IsSubclassOf (Type::Kind super);	
	Type::Kind parent;
	Type::Kind type;
	char *name;
	bool value_type;

	static void Shutdown ();
private:
	Type (char *name, Type::Kind type, Type::Kind parent);
	~Type ();
	static Type** types;
	static GHashTable *types_by_name;
	static void free_type (gpointer v);
};

bool type_get_value_type (Type::Kind type);

void types_init (void);

G_END_DECLS

#endif

