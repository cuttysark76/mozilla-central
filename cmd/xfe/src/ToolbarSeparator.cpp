/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * The contents of this file are subject to the Netscape Public License
 * Version 1.0 (the "NPL"); you may not use this file except in
 * compliance with the NPL.  You may obtain a copy of the NPL at
 * http://www.mozilla.org/NPL/
 *
 * Software distributed under the NPL is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the NPL
 * for the specific language governing rights and limitations under the
 * NPL.
 *
 * The Initial Developer of this code under the NPL is Netscape
 * Communications Corporation.  Portions created by Netscape are
 * Copyright (C) 1998 Netscape Communications Corporation.  All Rights
 * Reserved.
 */

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Name:        ToolbarSeparator.cpp                                    //
//                                                                      //
// Description:	XFE_ToolbarSeparator class implementation.              //
//              A toolbar item separator.                               //
//                                                                      //
// Author:		Ramiro Estrugo <ramiro@netscape.com>                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "ToolbarSeparator.h"

#include <Xm/Separator.h>

//////////////////////////////////////////////////////////////////////////
XFE_ToolbarSeparator::XFE_ToolbarSeparator(XFE_Frame *		frame,
										   Widget			parent,
										   HT_Resource		htResource,
										   const String		name) :
	XFE_ToolbarItem(frame,parent,htResource,name)
{
}
//////////////////////////////////////////////////////////////////////////
XFE_ToolbarSeparator::~XFE_ToolbarSeparator()
{
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Initialize
//
//////////////////////////////////////////////////////////////////////////
/* virtual */ void
XFE_ToolbarSeparator::initialize()
{
	Widget separator = createBaseWidget(getParent(),getName());

	setBaseWidget(separator);
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Sensitive interface
//
//////////////////////////////////////////////////////////////////////////
/* virtual */ void
XFE_ToolbarSeparator::setSensitive(Boolean state)
{
	XP_ASSERT( isAlive() );

	XtSetSensitive(m_widget,state);
}
//////////////////////////////////////////////////////////////////////////
/* virtual */ Boolean
XFE_ToolbarSeparator::isSensitive()
{
	XP_ASSERT( isAlive() );
	
	return XtIsSensitive(m_widget);
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Widget creation interface
//
//////////////////////////////////////////////////////////////////////////
/* virtual */ Widget
XFE_ToolbarSeparator::createBaseWidget(Widget		parent,
									   const String	name)
{
	XP_ASSERT( XfeIsAlive(parent) );
	XP_ASSERT( name != NULL );

	Widget separator;

	separator = XtVaCreateWidget(name,
								 xmSeparatorWidgetClass,
								 parent,
								 NULL);

	return separator;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Configure
//
//////////////////////////////////////////////////////////////////////////
/* virtual */ void
XFE_ToolbarSeparator::configure()
{
	XP_ASSERT( isAlive() );

	XtVaSetValues(m_widget,
				  XmNorientation,	XmVERTICAL,
				  NULL);
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Tool tip support
//
//////////////////////////////////////////////////////////////////////////
/* virtual */ void
XFE_ToolbarSeparator::tipStringObtain(XmString *	stringReturn,
								   Boolean *	needToFreeString)
{
// 	XP_ASSERT( isAlive() );
	
// 	*stringReturn = getTipStringFromAppDefaults();
// 	*needToFreeString = False;
}
//////////////////////////////////////////////////////////////////////////
/* virtual */ void
XFE_ToolbarSeparator::docStringObtain(XmString *	stringReturn,
								   Boolean *	needToFreeString)
{
// 	XP_ASSERT( isAlive() );
	
// 	*stringReturn = getDocStringFromAppDefaults();
// 	*needToFreeString = False;
}
//////////////////////////////////////////////////////////////////////////
/* virtual */ void
XFE_ToolbarSeparator::docStringSet(XmString /* string */)
{
// 	getAncestorFrame()->notifyInterested(Command::commandArmedCallback,
// 										 (void *) getCommand());
}
//////////////////////////////////////////////////////////////////////////
/* virtual */ void
XFE_ToolbarSeparator::docStringClear(XmString /* string */)
{
// 	getAncestorFrame()->notifyInterested(Command::commandDisarmedCallback,
// 										 (void *) getCommand());
}
//////////////////////////////////////////////////////////////////////////
