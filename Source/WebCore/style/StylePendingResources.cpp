/*
 * Copyright (C) 2016 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "StylePendingResources.h"

#include "CSSCursorImageValue.h"
#include "CachedResourceLoader.h"
#include "ContentData.h"
#include "CursorData.h"
#include "CursorList.h"
#include "Document.h"
#include "RenderStyle.h"
#include "SVGURIReference.h"
#include "StyleCachedImage.h"
#include "StyleGeneratedImage.h"
#include "TransformFunctions.h"

namespace WebCore {
namespace Style {

enum class LoadPolicy { Normal, ShapeOutside };
static void loadPendingImage(Document& document, const StyleImage* styleImage, const Element* element, LoadPolicy loadPolicy = LoadPolicy::Normal)
{
    if (!styleImage || !styleImage->isPending())
        return;

    ResourceLoaderOptions options = CachedResourceLoader::defaultCachedResourceOptions();
    options.contentSecurityPolicyImposition = element && element->isInUserAgentShadowTree() ? ContentSecurityPolicyImposition::SkipPolicyCheck : ContentSecurityPolicyImposition::DoPolicyCheck;

    // FIXME: Why does shape-outside have different policy than other properties?
    if (loadPolicy == LoadPolicy::ShapeOutside) {
        options.mode = FetchOptions::Mode::Cors;
        options.allowCredentials = DoNotAllowStoredCredentials;
        options.sameOriginDataURLFlag = SameOriginDataURLFlag::Set;
    }

    const_cast<StyleImage&>(*styleImage).load(document.cachedResourceLoader(), options);
}

void loadPendingResources(RenderStyle& style, Document& document, const Element* element)
{
    for (auto* backgroundLayer = style.backgroundLayers(); backgroundLayer; backgroundLayer = backgroundLayer->next())
        loadPendingImage(document, backgroundLayer->image(), element);

    for (auto* contentData = style.contentData(); contentData; contentData = contentData->next()) {
        if (is<ImageContentData>(*contentData)) {
            auto& styleImage = downcast<ImageContentData>(*contentData).image();
            loadPendingImage(document, &styleImage, element);
        }
    }

    if (auto* cursorList = style.cursors()) {
        for (size_t i = 0; i < cursorList->size(); ++i)
            loadPendingImage(document, cursorList->at(i).image(), element);
    }

    loadPendingImage(document, style.listStyleImage(), element);
    loadPendingImage(document, style.borderImageSource(), element);
    loadPendingImage(document, style.maskBoxImageSource(), element);

    if (auto* reflection = style.boxReflect())
        loadPendingImage(document, reflection->mask().image(), element);

    for (auto* maskLayer = style.maskLayers(); maskLayer; maskLayer = maskLayer->next())
        loadPendingImage(document, maskLayer->image(), element);

#if ENABLE(CSS_SHAPES)
    if (style.shapeOutside())
        loadPendingImage(document, style.shapeOutside()->image(), element, LoadPolicy::ShapeOutside);
#endif
}

}
}
