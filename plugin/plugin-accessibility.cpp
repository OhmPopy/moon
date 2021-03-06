/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * plugin-accessibility.cpp: DO subclass specifically for use as
 * content.accessibility.
 *
 * Contact:
 *   Moonlight List (moonlight-list@lists.ximian.com)
 *
 * Copyright 2009 Novell, Inc. (http://www.novell.com)
 *
 * See the LICENSE file included with the distribution for details.
 *
 */


#include <config.h>
#include "plugin-accessibility.h"
#include "moonlight.h"

namespace Moonlight {

Accessibility::Accessibility ()
{
	SetObjectType (Type::ACCESSIBILITY);
}

Accessibility::~Accessibility ()
{
}

void
Accessibility::PerformAction ()
{
	Emit (PerformActionEvent);
}
};