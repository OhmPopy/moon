/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * template.h:
 *
 * Copyright 2008 Novell, Inc. (http://www.novell.com)
 *
 * See the LICENSE file included with the distribution for details.
 * 
 */

#ifndef __MOON_TEMPLATE_H__
#define __MOON_TEMPLATE_H__

#include <glib.h>

#include "dependencyobject.h"
#include "control.h"

class TemplateBinding;
class XamlTemplateBinding;

//
// FrameworkTemplate
//
/* @SilverlightVersion="2" */
/* @Namespace=System.Windows */
class FrameworkTemplate : public DependencyObject {
public:
	/* @GenerateCBinding,GeneratePInvoke,ManagedAccess=Protected */
	FrameworkTemplate ();

	virtual Type::Kind GetObjectType () { return Type::FRAMEWORKTEMPLATE; }

	FrameworkElement *GetVisualTree () { return visual_tree; }
	void SetVisualTree (FrameworkElement* value);

	void AddXamlBinding (XamlTemplateBinding *binding);

protected:
	virtual ~FrameworkTemplate ();

	GHashTable *xaml_bindings;
	FrameworkElement *visual_tree;
};

//
// ControlTemplate
//
/* @SilverlightVersion="2" */
/* @Namespace=System.Windows.Controls */
class ControlTemplate : public FrameworkTemplate {
public:
	/* @PropertyType=Managed,ManagedPropertyType=System.Type,Access=Internal,ManagedAccessorAccess=Public,ManagedFieldAccess=Private */
	static DependencyProperty *TargetTypeProperty;
	
	/* @GenerateCBinding,GeneratePInvoke */
	ControlTemplate ();

	virtual Type::Kind GetObjectType () { return Type::CONTROLTEMPLATE; }

	FrameworkElement * Apply (Control *toControl, List *bindings);

protected:
	virtual ~ControlTemplate () {}

	DependencyObject* DuplicateObject (Control *source, DependencyObject *dob, List* bindings);

	static void duplicate_value (DependencyProperty *key, Value *value, gpointer closure);
};

/* @SilverlightVersion="2" */
class XamlTemplateBinding : public EventObject {
public:
	XamlTemplateBinding (FrameworkElement *target,
			     const char *targetPropertyName,
			     const char *sourcePropertyName);

	TemplateBinding *Attach (Control *source, FrameworkElement *target);

	FrameworkElement* GetTarget() { return target; }

protected:
	virtual ~XamlTemplateBinding ();

private:
	// this refers to the FWE in the template tree
	FrameworkElement *target;
	char *targetPropertyName;
	char *sourcePropertyName;
};

/* @SilverlightVersion="2" */
class TemplateBinding : public EventObject {
public:
	TemplateBinding (Control *source,
			 DependencyProperty *sourceProperty,
			 FrameworkElement *target,
			 DependencyProperty *targetProperty);
protected:
	~TemplateBinding ();

private:
	Control *source;
	DependencyProperty *sourceProperty;

	FrameworkElement *target;
	DependencyProperty *targetProperty;

	void OnSourcePropertyChanged (DependencyObject *sender, PropertyChangedEventArgs *args);
	static void SourcePropertyChangedCallback (DependencyObject *sender, PropertyChangedEventArgs *args, gpointer closure);
};

#endif /* __MOON_TEMPLATE_H__ */
