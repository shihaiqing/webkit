/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "SVGImageForContainer.h"

#include "AffineTransform.h"
#include "FloatRect.h"
#include "FloatSize.h"
#include "Image.h"

namespace WebCore {

FloatSize SVGImageForContainer::size() const
{
    FloatSize scaledContainerSize(m_containerSize);
    scaledContainerSize.scale(m_zoom);
    return FloatSize(roundedIntSize(scaledContainerSize));
}

void SVGImageForContainer::draw(GraphicsContext& context, const FloatRect& dstRect,
    const FloatRect& srcRect, CompositeOperator compositeOp, BlendMode blendMode, ImageOrientationDescription)
{
    m_image->drawForContainer(context, m_containerSize, m_zoom, dstRect, srcRect, compositeOp, blendMode);
}

void SVGImageForContainer::drawPattern(GraphicsContext& context, const FloatRect& dstRect, const FloatRect& srcRect, const AffineTransform& patternTransform,
    const FloatPoint& phase, const FloatSize& spacing, CompositeOperator compositeOp, BlendMode blendMode)
{
    m_image->drawPatternForContainer(context, m_containerSize, m_zoom, srcRect, patternTransform, phase, spacing, compositeOp, dstRect, blendMode);
}

NativeImagePtr SVGImageForContainer::nativeImageForCurrentFrame()
{
    return m_image->nativeImageForCurrentFrame();
}

} // namespace WebCore
