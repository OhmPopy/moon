/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * pal-linux-capture.cpp:
 *
 * Copyright 2010 Novell, Inc. (http://www.novell.com)
 *
 * See the LICENSE file included with the distribution for details.
 * 
 */

#include "config.h"

#include "pal-linux-capture.h"

#if PAL_V4L2_VIDEO_CAPTURE
#include "pal/capture/v4l2/pal-v4l2-video-capture.h"
#endif

MoonCaptureServiceLinux::MoonCaptureServiceLinux ()
{
	// FIXME we should do both compile time and runtime checking
	// for this.

#if PAL_V4L2_VIDEO_CAPTURE
	video_service = new MoonVideoCaptureServiceV4L2 ();
#else
	g_warning ("no video capture service available");
	video_service = NULL;
#endif

#if PAL_PULSE_AUDIO_CAPTURE
	audio_service = new MoonAudioCaptureServicePulse ();
#else
	g_warning ("no audio capture service available");
	audio_service = NULL;
#endif
}

MoonCaptureServiceLinux::~MoonCaptureServiceLinux ()
{
	delete video_service;
	delete audio_service;
}

MoonVideoCaptureService* 
MoonCaptureServiceLinux::GetVideoCaptureService()
{
	return video_service;
}

MoonAudioCaptureService*
MoonCaptureServiceLinux::GetAudioCaptureService()
{
	return audio_service;
}

bool
MoonCaptureServiceLinux::RequiresSystemPermissionForDeviceAccess ()
{
	return false;
}

bool
MoonCaptureServiceLinux::RequestSystemAccess ()
{
	// FIXME need to figure this out - we need to use the pal windowing system to pop up a dialog
	return true;
}
