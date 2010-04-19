/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * provider.cpp: an api for PropertyValue providers (for property inheritance)
 *
 * Copyright 2007 Novell, Inc. (http://www.novell.com)
 *
 * See the LICENSE file included with the distribution for details.
 * 
 */

#include <config.h>

#include "cbinding.h"
#include "runtime.h"
#include "provider.h"
#include "control.h"
#include "frameworkelement.h"
#include "textblock.h"
#include "style.h"
#include "deployment.h"
#include "error.h"

//
// LocalPropertyValueProvider
//

LocalPropertyValueProvider::LocalPropertyValueProvider (DependencyObject *obj, PropertyPrecedence precedence)
	: PropertyValueProvider (obj, precedence)
{
	// XXX maybe move the "DependencyObject::current_values" hash table here?
}

LocalPropertyValueProvider::~LocalPropertyValueProvider ()
{
}

Value *
LocalPropertyValueProvider::GetPropertyValue (DependencyProperty *property)
{
	return (Value *) g_hash_table_lookup (obj->GetLocalValues (), property);
}


//
// StylePropertyValueProvider
//

StylePropertyValueProvider::StylePropertyValueProvider (DependencyObject *obj, PropertyPrecedence precedence)
	: PropertyValueProvider (obj, precedence)
{
	style = NULL;
	style_hash = g_hash_table_new_full (g_direct_hash, g_direct_equal,
					    (GDestroyNotify)NULL,
					    (GDestroyNotify)NULL);
}

void
StylePropertyValueProvider::unlink_converted_value (gpointer key, gpointer value, gpointer data)
{
	StylePropertyValueProvider *provider = (StylePropertyValueProvider*)data;
	Value *v = (Value*)value;

	if (v && v->Is(Deployment::GetCurrent (), Type::DEPENDENCY_OBJECT)) {
		DependencyObject *dob = v->AsDependencyObject();
		if (dob->GetParent() == provider->obj)
			dob->SetParent(NULL, NULL);
	}
}

StylePropertyValueProvider::~StylePropertyValueProvider ()
{
	g_hash_table_foreach (style_hash, StylePropertyValueProvider::unlink_converted_value, this);
	g_hash_table_destroy (style_hash);
}

Value*
StylePropertyValueProvider::GetPropertyValue (DependencyProperty *property)
{
	return (Value *) g_hash_table_lookup (style_hash, property);
}

void
StylePropertyValueProvider::RecomputePropertyValue (DependencyProperty *prop, MoonError *error)
{
	Value *old_value = NULL;
	Value *new_value = NULL;
	DependencyProperty *property = NULL;

	DeepStyleWalker walker (style);
	while (Setter *setter = walker.Step ()) {
		property = setter->GetValue (Setter::PropertyProperty)->AsDependencyProperty ();
		if (property != prop)
			continue;

		new_value = setter->GetValue (Setter::ConvertedValueProperty);

		setter->ref ();
		old_value = (Value *) g_hash_table_lookup (style_hash, property);

		g_hash_table_insert (style_hash, property, new_value);

		obj->ProviderValueChanged (precedence, property, old_value, new_value, true, true, true, error);
		if (error->number)
			return;
	}
}

void
StylePropertyValueProvider::UpdateStyle (Style *style, MoonError *error)
{
	Value *oldValue;
	Value *newValue;

	DeepStyleWalker oldWalker (this->style);
	DeepStyleWalker newWalker (style);

	Setter *oldSetter = oldWalker.Step ();
	Setter *newSetter = newWalker.Step ();

	while (oldSetter || newSetter) {

		DependencyProperty *oldProp = NULL;
		DependencyProperty *newProp = NULL;

		if (oldSetter)
			oldProp = oldSetter->GetValue (Setter::PropertyProperty)->AsDependencyProperty ();
		if (newSetter)
			newProp = newSetter->GetValue (Setter::PropertyProperty)->AsDependencyProperty ();
		
		if (oldProp != NULL && (oldProp < newProp || newProp == NULL)) {
			// this is a property in the old style which is not in the new style
			oldValue = oldSetter->GetValue (Setter::ConvertedValueProperty);
			newValue = NULL;

			g_hash_table_remove (style_hash, oldProp);
			obj->ProviderValueChanged (precedence, oldProp, oldValue, newValue, true, true, false, error);

			oldSetter = oldWalker.Step ();
		} else if (oldProp == newProp) {
			// This is a property which is in both styles
			oldValue = oldSetter->GetValue (Setter::ConvertedValueProperty);
			newValue = newSetter->GetValue (Setter::ConvertedValueProperty);

			g_hash_table_insert (style_hash, oldProp, newValue);
			obj->ProviderValueChanged (precedence, oldProp, oldValue, newValue, true, true, false, error);

			oldSetter = oldWalker.Step ();
			newSetter = newWalker.Step ();
		} else {
			// This is a property which is only in the new style
			oldValue = NULL;
			newValue = newSetter->GetValue (Setter::ConvertedValueProperty);

			g_hash_table_insert (style_hash, newProp, newValue);
			obj->ProviderValueChanged (precedence, newProp, oldValue, newValue, true, true, false, error);

			newSetter = newWalker.Step ();
		}
	}

	this->style = style;
}

//
// InheritedPropertyValueProvider
//
Value*
InheritedPropertyValueProvider::GetPropertyValue (DependencyProperty *property)
{
	int propertyId = property->GetId ();

	if (!IsPropertyInherited (propertyId))
		return NULL;

	int parentPropertyId = -1;

	Types *types =  Deployment::GetCurrent()->GetTypes();

#define INHERIT_CTI_CTI(p) \
	G_STMT_START {							\
	if (property->GetId () == Control::p ||				\
	    property->GetId () == TextBlock::p ||			\
	    property->GetId () == Inline::p) {				\
									\
		if (types->IsSubclassOf (parent->GetObjectType(), Type::CONTROL)) \
			parentPropertyId = Control::p;			\
		else if (types->IsSubclassOf (parent->GetObjectType(), Type::TEXTBLOCK)) \
			parentPropertyId = TextBlock::p;		\
	}								\
	} G_STMT_END


#define INHERIT_I_T(p) \
	G_STMT_START {							\
	if (property->GetId () == Inline::p) {				\
		parentPropertyId = TextBlock::p;			\
	}								\
	} G_STMT_END

#define INHERIT_F_F(p) \
	G_STMT_START {							\
	if (property->GetId () == FrameworkElement::p) {		\
		parentPropertyId = FrameworkElement::p;			\
	}								\
	} G_STMT_END

#define INHERIT_U_U(p) \
	G_STMT_START {							\
	if (property->GetId () == UIElement::p) {		\
		parentPropertyId = UIElement::p;			\
	}								\
	} G_STMT_END
	
	DependencyObject *parent = NULL;

	if (types->IsSubclassOf (obj->GetObjectType(), Type::FRAMEWORKELEMENT)) {
		// we loop up the visual tree
		parent = ((FrameworkElement*)obj)->GetVisualParent();
		if (parent) {
			while (parent) {
				INHERIT_CTI_CTI (ForegroundProperty);
				INHERIT_CTI_CTI (FontFamilyProperty);
				INHERIT_CTI_CTI (FontStretchProperty);
				INHERIT_CTI_CTI (FontStyleProperty);
				INHERIT_CTI_CTI (FontWeightProperty);
				INHERIT_CTI_CTI (FontSizeProperty);

				INHERIT_F_F (LanguageProperty);
				
				INHERIT_U_U (UseLayoutRoundingProperty);

				if (parentPropertyId != -1)
					return parent->GetValue (types->GetProperty (parentPropertyId));

				parent = ((FrameworkElement*)parent)->GetVisualParent();
			}
		}
	}
	else if (types->IsSubclassOf (obj->GetObjectType(), Type::INLINE)) {
		// skip collections
		DependencyObject *new_parent = obj->GetParent();
		while (new_parent && !types->IsSubclassOf (new_parent->GetObjectType(), Type::TEXTBLOCK))
			new_parent = new_parent->GetParent ();
		parent = new_parent;

		if (!parent)
			return NULL;

		INHERIT_I_T (ForegroundProperty);
		INHERIT_I_T (FontFamilyProperty);
		INHERIT_I_T (FontStretchProperty);
		INHERIT_I_T (FontStyleProperty);
		INHERIT_I_T (FontWeightProperty);
		INHERIT_I_T (FontSizeProperty);

		INHERIT_I_T (LanguageProperty);
		INHERIT_I_T (TextDecorationsProperty);
		
		if (parentPropertyId != -1) {
			return parent->GetValue (parentPropertyId);
		}
	}
	  
	return NULL;
}


bool
InheritedPropertyValueProvider::IsPropertyInherited (int propertyId)
{
#define PROP_CTI(p) G_STMT_START { \
	if (propertyId == Control::p) return true; \
	if (propertyId == TextBlock::p) return true; \
	if (propertyId == Inline::p) return true; \
	} G_STMT_END
		
#define PROP_F(p) G_STMT_START { \
	if (propertyId == FrameworkElement::p) return true; \
	} G_STMT_END

#define PROP_U(p) G_STMT_START { \
	if (propertyId == UIElement::p) return true; \
        } G_STMT_END

#define PROP_I(p) G_STMT_START { \
	if (propertyId == Inline::p) return true; \
	} G_STMT_END

	PROP_CTI (ForegroundProperty);
	PROP_CTI (FontFamilyProperty);
	PROP_CTI (FontStretchProperty);
	PROP_CTI (FontStyleProperty);
	PROP_CTI (FontWeightProperty);
	PROP_CTI (FontSizeProperty);

	PROP_U (UseLayoutRoundingProperty);

	PROP_F (LanguageProperty);

	PROP_I (LanguageProperty);
	PROP_I (TextDecorationsProperty);

	return false;
}

DependencyProperty*
InheritedPropertyValueProvider::MapPropertyToDescendant (Types *types,
							 DependencyProperty *property,
							 Type::Kind descendantKind)
{
#define PROPAGATE_C_CT(p) G_STMT_START {				\
		if (property->GetId() == Control::p) {			\
			if (types->IsSubclassOf (descendantKind, Type::CONTROL)) \
				return types->GetProperty (Control::p); \
			else if (types->IsSubclassOf (descendantKind, Type::TEXTBLOCK)) \
				return types->GetProperty (TextBlock::p); \
		}							\
	} G_STMT_END

#define PROPAGATE_T_I(p) G_STMT_START {					\
		if (property->GetId() == TextBlock::p) {		\
			/* we don't need the check here since we can do it once above all the PROPAGATE_I below */ \
			/*if (types->IsSubclassOf (descendantKind, Type::INLINE))*/ \
				return types->GetProperty (Inline::p); \
		}							\
	} G_STMT_END

	if (types->IsSubclassOf (property->GetOwnerType(), Type::CONTROL)) {
		PROPAGATE_C_CT (ForegroundProperty);
		PROPAGATE_C_CT (FontFamilyProperty);
		PROPAGATE_C_CT (FontStretchProperty);
		PROPAGATE_C_CT (FontStyleProperty);
		PROPAGATE_C_CT (FontWeightProperty);
		PROPAGATE_C_CT (FontSizeProperty);
	}

	if (types->IsSubclassOf (property->GetOwnerType(), Type::TEXTBLOCK)) {
		if (types->IsSubclassOf (descendantKind, Type::INLINE)) {
			PROPAGATE_T_I (ForegroundProperty);
			PROPAGATE_T_I (FontFamilyProperty);
			PROPAGATE_T_I (FontStretchProperty);
			PROPAGATE_T_I (FontStyleProperty);
			PROPAGATE_T_I (FontWeightProperty);
			PROPAGATE_T_I (FontSizeProperty);

			PROPAGATE_T_I (LanguageProperty);
			PROPAGATE_T_I (TextDecorationsProperty);
		}
	}

	if (types->IsSubclassOf (property->GetOwnerType(), Type::FRAMEWORKELEMENT)) {
		if (types->IsSubclassOf (descendantKind, Type::FRAMEWORKELEMENT)) {
			if (property->GetId() == FrameworkElement::LanguageProperty)
				return property;
		}
	}

	if (types->IsSubclassOf (property->GetOwnerType(), Type::UIELEMENT)) {
		if (types->IsSubclassOf (descendantKind, Type::UIELEMENT)) {
			if (property->GetId() == UIElement::UseLayoutRoundingProperty)
				return property;
		}
	}

	return NULL;
}

void
InheritedPropertyValueProvider::PropagateInheritedProperty (DependencyObject *obj, DependencyProperty *property, Value *old_value, Value *new_value)
{
	Types *types = obj->GetDeployment ()->GetTypes ();

	if (types->IsSubclassOf (obj->GetObjectType(), Type::TEXTBLOCK)) {
		InlineCollection *inlines = ((TextBlock*)obj)->GetInlines();

		// lift this out of the loop since we know all
		// elements of InlinesProperty will be inlines.
		DependencyProperty *child_property = MapPropertyToDescendant (types, property, Type::INLINE);
		if (!child_property)
			return;

		int inlines_count = inlines->GetCount ();
		for (int i = 0; i < inlines_count; i++) {
			Inline *item = inlines->GetValueAt (i)->AsInline ();

			MoonError error;

			item->ProviderValueChanged (PropertyPrecedence_Inherited, child_property,
						    old_value, new_value, false, false, false, &error);

			if (error.number) {
				// FIXME: what do we do here?  I'm guessing we continue propagating?
			}
		}

	}
	else {
		// for inherited properties, we need to walk down the
		// subtree and call ProviderValueChanged on all
		// elements that can inherit the property.
		DeepTreeWalker walker ((UIElement*)obj, Logical, types);

		walker.Step (); // skip obj

		while (UIElement *element = walker.Step ()) {
			DependencyProperty *child_property = MapPropertyToDescendant (types, property, element->GetObjectType());
			if (!child_property)
				continue;

			MoonError error;

			element->ProviderValueChanged (PropertyPrecedence_Inherited, child_property,
						       old_value, new_value, true, false, false, &error);

			if (error.number) {
				// FIXME: what do we do here?  I'm guessing we continue propagating?
			}

			walker.SkipBranch ();
		}
	}
}

#define FOREGROUND_PROP     (1<<0)
#define FONTFAMILY_PROP     (1<<1)
#define FONTSTRETCH_PROP    (1<<2)
#define FONTSTYLE_PROP      (1<<3)
#define FONTWEIGHT_PROP     (1<<4)
#define FONTSIZE_PROP       (1<<5)
#define LANGUAGE_PROP       (1<<6)
#define LAYOUTROUNDING_PROP (1<<8)

#define HAS_SEEN(s,p)  (((s) & (p))!=0)
#define SEEN(s,p)      ((s)|=(p))

#define PROP_ADD(p,s) G_STMT_START {					\
	if (!HAS_SEEN (seen, s)) {					\
		DependencyProperty *property = types->GetProperty (p);		\
		Value *v = element->GetValue (property, PropertyPrecedence_Inherited, PropertyPrecedence_Inherited); \
		if (v != NULL) {					\
			element->ProviderValueChanged (PropertyPrecedence_Inherited, property, \
						       NULL, v,		\
						       true, false, false, &error); \
			SEEN (seen, s);					\
		}							\
	}								\
	} G_STMT_END

static void
walk_tree (Types *types, UIElement *element, guint32 seen)
{
	MoonError error;

	if (types->IsSubclassOf (element->GetObjectType (), Type::CONTROL)) {
		PROP_ADD (Control::ForegroundProperty, FOREGROUND_PROP);
		PROP_ADD (Control::FontFamilyProperty, FONTFAMILY_PROP);
		PROP_ADD (Control::FontStretchProperty, FONTSTRETCH_PROP);
		PROP_ADD (Control::FontStyleProperty, FONTSTYLE_PROP);
		PROP_ADD (Control::FontWeightProperty, FONTWEIGHT_PROP);
		PROP_ADD (Control::FontSizeProperty, FONTSIZE_PROP);
	}

	if (types->IsSubclassOf (element->GetObjectType (), Type::TEXTBLOCK)) {
		PROP_ADD (TextBlock::ForegroundProperty, FOREGROUND_PROP);
		PROP_ADD (TextBlock::FontFamilyProperty, FONTFAMILY_PROP);
		PROP_ADD (TextBlock::FontStretchProperty, FONTSTRETCH_PROP);
		PROP_ADD (TextBlock::FontStyleProperty, FONTSTYLE_PROP);
		PROP_ADD (TextBlock::FontWeightProperty, FONTWEIGHT_PROP);
		PROP_ADD (TextBlock::FontSizeProperty, FONTSIZE_PROP);
	}

	if (types->IsSubclassOf (element->GetObjectType (), Type::FRAMEWORKELEMENT)) {
		PROP_ADD (FrameworkElement::LanguageProperty, LANGUAGE_PROP);
	}

	
	PROP_ADD (UIElement::UseLayoutRoundingProperty, LAYOUTROUNDING_PROP);

	VisualTreeWalker walker ((UIElement*)element, Logical, types);

	while (UIElement *child = walker.Step ())
		walk_tree (types, child, seen);

}

void
InheritedPropertyValueProvider::PropagateInheritedPropertiesOnAddingToTree (UIElement *subtreeRoot)
{
	Types *types = subtreeRoot->GetDeployment ()->GetTypes ();

	walk_tree (types, subtreeRoot, 0);
}

//
// AutoPropertyValueProvider
//

static gboolean
dispose_value (gpointer key, gpointer value, gpointer data)
{
	DependencyObject *obj = (DependencyObject *) data;
	Value *v = (Value *) value;
	
	if (!value)
		return true;
	
	// detach from the existing value
	if (v->Is (obj->GetDeployment (), Type::DEPENDENCY_OBJECT)) {
		DependencyObject *dob = v->AsDependencyObject ();
		
		if (dob != NULL) {
			if (obj == dob->GetParent ()) {
				// unset its logical parent
				dob->SetParent (NULL, NULL);
			}
			
			// unregister from the existing value
			dob->RemovePropertyChangeListener (obj, NULL);
		}
	}
	
	delete v;
	
	return true;
}

AutoCreatePropertyValueProvider::AutoCreatePropertyValueProvider (DependencyObject *obj, PropertyPrecedence precedence)
	: PropertyValueProvider (obj, precedence)
{
	auto_values = g_hash_table_new (g_direct_hash, g_direct_equal);
}

AutoCreatePropertyValueProvider::~AutoCreatePropertyValueProvider ()
{
	g_hash_table_foreach_remove (auto_values, dispose_value, obj);
	g_hash_table_destroy (auto_values);
}

Value *
AutoCreatePropertyValueProvider::GetPropertyValue (DependencyProperty *property)
{
	Value *value;
	
	// return previously set auto value next
	if ((value = (Value *) g_hash_table_lookup (auto_values, property)))
		return value;
	
	if (!(value = property->GetDefaultValue (obj->GetObjectType ())))
		return NULL;

#if SANITY
	Deployment *deployment = Deployment::GetCurrent ();
	if (!value->Is(deployment, property->GetPropertyType()))
		g_warning ("autocreated value for property '%s' (type=%s) is of incompatible type %s\n",
			   property->GetName(),
			   Type::Find (deployment, property->GetPropertyType ())->GetName(),
			   Type::Find (deployment, value->GetKind())->GetName());
#endif

	g_hash_table_insert (auto_values, property, value);
	
	MoonError error;
	obj->ProviderValueChanged (precedence, property, NULL, value, false, true, false, &error);
	
	return value;
}

Value *
AutoCreatePropertyValueProvider::ReadLocalValue (DependencyProperty *property)
{
	return (Value *) g_hash_table_lookup (auto_values, property);
}

void
AutoCreatePropertyValueProvider::ClearValue (DependencyProperty *property)
{
	g_hash_table_remove (auto_values, property);
}

Value* 
AutoCreators::default_autocreator (Type::Kind kind, DependencyProperty *property)
{
	Type *type = Type::Find (Deployment::GetCurrent (), property->GetPropertyType ());
	if (!type)
		return NULL;

	return Value::CreateUnrefPtr (type->CreateInstance ());
}

#define XAML_FONT_SIZE    14.666666984558105
#define XAP_FONT_SIZE     11.0

Value *
AutoCreators::CreateDefaultFontSize (Type::Kind kind, DependencyProperty *property)
{
	Deployment *deployment;
	
	if ((deployment = Deployment::GetCurrent ()) && deployment->IsLoadedFromXap ())
		return new Value (XAP_FONT_SIZE);
	
	return new Value (XAML_FONT_SIZE);
}

Value *
AutoCreators::CreateBlackBrush (Type::Kind kind, DependencyProperty *property)
{
	SolidColorBrush *brush = new SolidColorBrush ("black");
	brush->Freeze ();
	return Value::CreateUnrefPtr (brush);
}

InheritedDataContextValueProvider::InheritedDataContextValueProvider (DependencyObject *obj, PropertyPrecedence precedence)
	: PropertyValueProvider (obj, precedence)
{
	source = NULL;
}

InheritedDataContextValueProvider::~InheritedDataContextValueProvider ()
{
	DetachListener ();
	source = NULL;
}

Value *
InheritedDataContextValueProvider::GetPropertyValue (DependencyProperty *property)
{
	if (source == NULL || property->GetId () != FrameworkElement::DataContextProperty)
		return NULL;
	return source->GetValue (property);
}

void
InheritedDataContextValueProvider::AttachListener ()
{
	if (source != NULL) {
		DependencyProperty *prop = obj->GetDeployment ()->GetTypes ()->GetProperty (FrameworkElement::DataContextProperty);
		source->AddPropertyChangeHandler (prop, source_data_context_changed, this);
		source->AddHandler (EventObject::DestroyedEvent, source_destroyed, this);
	}
}
void
InheritedDataContextValueProvider::DetachListener ()
{
	if (source != NULL) {
		DependencyProperty *prop = obj->GetDeployment ()->GetTypes ()->GetProperty (FrameworkElement::DataContextProperty);
		source->RemovePropertyChangeHandler (prop, source_data_context_changed);
		source->RemoveHandler (EventObject::DestroyedEvent, source_destroyed, this);
	}
}

void
InheritedDataContextValueProvider::SetDataSource (FrameworkElement *source)
{
	Value *old_value = this->source ? this->source->GetValue (FrameworkElement::DataContextProperty) : NULL;
	Value *new_value = source ? source->GetValue (FrameworkElement::DataContextProperty) : NULL;

	DetachListener ();
	this->source = source;
	AttachListener ();

	// This is kinda hacky - silverlight doesn't notify the listeners that the datacontext changed in this situation.
	// It waits for the element to be loaded and fires the event when that happens.
	if (old_value != new_value) {
		MoonError error;
		DependencyProperty *prop = obj->GetDeployment ()->GetTypes ()->GetProperty (FrameworkElement::DataContextProperty);
		obj->ProviderValueChanged (precedence, prop, old_value, new_value, false, false, false, &error);
	}
}

void
InheritedDataContextValueProvider::EmitChanged ()
{
	// This method exists solely so that we can notify any listeners that the inherited datacontext has changed
	// once an element is added to the live tree. This is required so that bindings refresh at the correct time.
	if (source != NULL) {
		DependencyProperty *prop = obj->GetDeployment ()->GetTypes ()->GetProperty (FrameworkElement::DataContextProperty);
		MoonError error;
		obj->ProviderValueChanged (precedence, prop, NULL, source->GetValue (prop), true, false, false, &error);
	}
}

void
InheritedDataContextValueProvider::source_data_context_changed (DependencyObject *sender, PropertyChangedEventArgs *args, MoonError *error, gpointer closure)
{
	InheritedDataContextValueProvider *p = (InheritedDataContextValueProvider *) closure;
	p->obj->ProviderValueChanged (p->precedence, args->GetProperty (), args->GetOldValue (), args->GetNewValue (), true, false, false, error);
}

void
InheritedDataContextValueProvider::source_destroyed (EventObject *sender, EventArgs *args, gpointer closure)
{
	InheritedDataContextValueProvider *p = (InheritedDataContextValueProvider *) closure;
	p->SetDataSource (NULL);
}
