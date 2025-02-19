/*
 * Copyright (C) 2003, 2006, 2013 Apple Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "Logging.h"
#include "LogInitialization.h"

#include <wtf/StdLibExtras.h>
#include <wtf/text/CString.h>
#include <wtf/text/WTFString.h>

#if PLATFORM(COCOA)
#include <notify.h>
#endif

namespace WebCore {

#if !LOG_DISABLED || !RELEASE_LOG_DISABLED

#define DEFINE_WEBCORE_LOG_CHANNEL(name) DEFINE_LOG_CHANNEL(name, LOG_CHANNEL_WEBKIT_SUBSYSTEM)
WEBCORE_LOG_CHANNELS(DEFINE_WEBCORE_LOG_CHANNEL)

static WTFLogChannel* logChannels[] = {
    WEBCORE_LOG_CHANNELS(LOG_CHANNEL_ADDRESS)
};

static const size_t logChannelCount = WTF_ARRAY_LENGTH(logChannels);

bool isLogChannelEnabled(const String& name)
{
    WTFLogChannel* channel = WTFLogChannelByName(logChannels, logChannelCount, name.utf8().data());
    if (!channel)
        return false;
    return channel->state != WTFLogChannelOff;
}

static bool logChannelsNeedInitialization = true;

void setLogChannelToAccumulate(const String& name)
{
    WTFLogChannel* channel = WTFLogChannelByName(logChannels, logChannelCount, name.utf8().data());
    if (!channel)
        return;

    channel->state = WTFLogChannelOnWithAccumulation;
    logChannelsNeedInitialization = true;
}

void initializeLogChannelsIfNecessary()
{
    if (!logChannelsNeedInitialization)
        return;

    logChannelsNeedInitialization = false;

    WTFInitializeLogChannelStatesFromString(logChannels, logChannelCount, logLevelString().utf8().data());
}

#ifndef NDEBUG
void registerNotifyCallback(const String& notifyID, std::function<void()> callback)
{
#if PLATFORM(COCOA)
    int token;
    notify_register_dispatch(notifyID.utf8().data(), &token, dispatch_get_main_queue(), ^(int) {
        callback();
    });
#else
    UNUSED_PARAM(notifyID);
    UNUSED_PARAM(callback);
#endif
}
#endif

#endif // !LOG_DISABLED || !RELEASE_LOG_DISABLED

#if !LOG_DISABLED
void logFunctionResult(WTFLogChannel* channel, std::function<const char*()> function)
{
    WTFLog(channel, "%s", function());
}

#endif // !LOG_DISABLED

} // namespace WebCore
