/*
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "MediaQueryMatcher.h"

#include "Document.h"
#include "Frame.h"
#include "FrameView.h"
#include "MediaList.h"
#include "MediaQueryEvaluator.h"
#include "MediaQueryList.h"
#include "MediaQueryListListener.h"
#include "NodeRenderStyle.h"
#include "RenderElement.h"
#include "StyleResolver.h"
#include "StyleScope.h"

namespace WebCore {

MediaQueryMatcher::MediaQueryMatcher(Document& document)
    : m_document(&document)
{
}

MediaQueryMatcher::~MediaQueryMatcher()
{
}

void MediaQueryMatcher::documentDestroyed()
{
    m_listeners.clear();
    m_document = nullptr;
}

String MediaQueryMatcher::mediaType() const
{
    if (!m_document || !m_document->frame() || !m_document->frame()->view())
        return String();

    return m_document->frame()->view()->mediaType();
}

std::unique_ptr<RenderStyle> MediaQueryMatcher::documentElementUserAgentStyle() const
{
    if (!m_document || !m_document->frame())
        return nullptr;

    auto* documentElement = m_document->documentElement();
    if (!documentElement)
        return nullptr;

    return m_document->styleScope().resolver().styleForElement(*documentElement, m_document->renderStyle(), MatchOnlyUserAgentRules).renderStyle;
}

bool MediaQueryMatcher::evaluate(const MediaQuerySet& media)
{
    auto style = documentElementUserAgentStyle();
    if (!style)
        return false;
    return MediaQueryEvaluator { mediaType(), *m_document, style.get() }.evaluate(media);
}

RefPtr<MediaQueryList> MediaQueryMatcher::matchMedia(const String& query)
{
    if (!m_document)
        return nullptr;

    auto media = MediaQuerySet::create(query);
    reportMediaQueryWarningIfNeeded(m_document, media.ptr());
    bool result = evaluate(media.get());
    return MediaQueryList::create(*this, WTFMove(media), result);
}

void MediaQueryMatcher::addListener(Ref<MediaQueryListListener>&& listener, MediaQueryList& query)
{
    if (!m_document)
        return;

    for (auto& existingListener : m_listeners) {
        if (existingListener.listener.get() == listener.get() && existingListener.query.ptr() == &query)
            return;
    }

    m_listeners.append(Listener { WTFMove(listener), query });
}

void MediaQueryMatcher::removeListener(MediaQueryListListener& listener, MediaQueryList& query)
{
    m_listeners.removeFirstMatching([&listener, &query](auto& existingListener) {
        return existingListener.listener.get() == listener && existingListener.query.ptr() == &query;
    });
}

void MediaQueryMatcher::styleResolverChanged()
{
    ASSERT(m_document);

    ++m_evaluationRound;

    auto style = documentElementUserAgentStyle();
    if (!style)
        return;

    MediaQueryEvaluator evaluator { mediaType(), *m_document, style.get() };
    Vector<Listener> listeners;
    listeners.reserveInitialCapacity(m_listeners.size());
    for (auto& listener : m_listeners)
        listeners.uncheckedAppend({ listener.listener.copyRef(), listener.query.copyRef() });
    for (auto& listener : listeners) {
        bool notify;
        listener.query->evaluate(evaluator, notify);
        if (notify)
            listener.listener->queryChanged(listener.query.ptr());
    }
}

} // namespace WebCore
