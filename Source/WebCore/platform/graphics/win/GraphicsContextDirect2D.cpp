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
#include "GraphicsContext.h"

#include "COMPtr.h"
#include "CurrentTime.h"
#include "DisplayListRecorder.h"
#include "FloatRoundedRect.h"
#include "GraphicsContextPlatformPrivateDirect2D.h"
#include "ImageBuffer.h"
#include "Logging.h"
#include "NotImplemented.h"
#include "URL.h"
#include <d2d1.h>
#include <d2d1effects.h>
#include <dwrite.h>

#pragma warning (disable : 4756)

using namespace std;

namespace WebCore {

GraphicsContext::GraphicsContext(HDC hdc, bool hasAlpha)
{
    platformInit(hdc, hasAlpha);
}

GraphicsContext::GraphicsContext(HDC hdc, ID2D1DCRenderTarget** renderTarget, RECT rect, bool hasAlpha)
{
    m_data->m_hdc = hdc;

    // Create a DC render target.
    auto targetProperties = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
        0, 0, D2D1_RENDER_TARGET_USAGE_NONE, D2D1_FEATURE_LEVEL_DEFAULT);

    HRESULT hr = GraphicsContext::systemFactory()->CreateDCRenderTarget(&targetProperties, renderTarget);
    RELEASE_ASSERT(SUCCEEDED(hr));

    (*renderTarget)->BindDC(hdc, &rect);

    m_data = new GraphicsContextPlatformPrivate(*renderTarget);
}

ID2D1Factory* GraphicsContext::systemFactory()
{
    static ID2D1Factory* direct2DFactory = nullptr;
    if (!direct2DFactory) {
        HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &direct2DFactory);
        RELEASE_ASSERT(SUCCEEDED(hr));
    }

    return direct2DFactory;
}

ID2D1RenderTarget* GraphicsContext::defaultRenderTarget()
{
    static ID2D1RenderTarget* defaultRenderTarget = nullptr;
    if (!defaultRenderTarget) {
        auto renderTargetProperties = D2D1::RenderTargetProperties();
        renderTargetProperties.usage = D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE;
        auto hwndRenderTargetProperties = D2D1::HwndRenderTargetProperties(::GetDesktopWindow(), D2D1::SizeU(10, 10));
        HRESULT hr = systemFactory()->CreateHwndRenderTarget(&renderTargetProperties, &hwndRenderTargetProperties, reinterpret_cast<ID2D1HwndRenderTarget**>(&defaultRenderTarget));
        RELEASE_ASSERT(SUCCEEDED(hr));
    }

    return defaultRenderTarget;
}

void GraphicsContext::setDidBeginDraw(bool didBeginDraw)
{
    RELEASE_ASSERT(m_data);
    m_data->m_didBeginDraw = didBeginDraw;
}

bool GraphicsContext::didBeginDraw() const
{
    return m_data->m_didBeginDraw;
}

void GraphicsContext::platformInit(HDC hdc, bool hasAlpha)
{
    if (!hdc)
        return;

    COMPtr<ID2D1RenderTarget> renderTarget;
    m_data = new GraphicsContextPlatformPrivate(renderTarget.get());
    m_data->m_hdc = hdc;
    // Make sure the context starts in sync with our state.
    setPlatformFillColor(fillColor());
    setPlatformStrokeColor(strokeColor());
    setPlatformStrokeThickness(strokeThickness());
    // FIXME: m_state.imageInterpolationQuality = convertInterpolationQuality(CGContextGetInterpolationQuality(platformContext()));
}

void GraphicsContext::platformInit(ID2D1RenderTarget* renderTarget)
{
    if (!renderTarget)
        return;

    m_data = new GraphicsContextPlatformPrivate(renderTarget);

    // Make sure the context starts in sync with our state.
    setPlatformFillColor(fillColor());
    setPlatformStrokeColor(strokeColor());
    setPlatformStrokeThickness(strokeThickness());
    // FIXME: m_state.imageInterpolationQuality = convertInterpolationQuality(CGContextGetInterpolationQuality(platformContext()));
}

void GraphicsContext::platformDestroy()
{
    delete m_data;
}

ID2D1RenderTarget* GraphicsContext::platformContext() const
{
    ASSERT(!paintingDisabled());
    ASSERT(m_data->renderTarget());
    return m_data->renderTarget();
}

void GraphicsContext::savePlatformState()
{
    ASSERT(!paintingDisabled());
    ASSERT(!isRecording());

    // Note: Do not use this function within this class implementation, since we want to avoid the extra
    // save of the secondary context (in GraphicsContextPlatformPrivateDirect2D.h).
    m_data->save();
}

void GraphicsContext::restorePlatformState()
{
    ASSERT(!paintingDisabled());
    ASSERT(!isRecording());

    // Note: Do not use this function within this class implementation, since we want to avoid the extra
    // restore of the secondary context (in GraphicsContextPlatformPrivateDirect2D.h).
    m_data->restore();
    // FIXME: m_data->m_userToDeviceTransformKnownToBeIdentity = false;
}

void GraphicsContext::drawNativeImage(const COMPtr<ID2D1Bitmap>& image, const FloatSize& imageSize, const FloatRect& destRect, const FloatRect& srcRect, CompositeOperator op, BlendMode blendMode, ImageOrientation orientation)
{
    if (paintingDisabled())
        return;

    if (isRecording()) {
        // FIXME: Implement DisplayListRecorder support for drawNativeImage.
        // m_displayListRecorder->drawNativeImage(image, imageSize, destRect, srcRect, op, blendMode, orientation);
        notImplemented();
        return;
    }

    auto bitmapSize = image->GetSize();

    float currHeight = orientation.usesWidthAsHeight() ? bitmapSize.width : bitmapSize.height;
    if (currHeight <= srcRect.y())
        return;

    auto context = platformContext();

    D2D1_MATRIX_3X2_F ctm;
    context->GetTransform(&ctm);

    AffineTransform transform(ctm);

    D2DContextStateSaver stateSaver(*m_data);

    bool shouldUseSubimage = false;

    // If the source rect is a subportion of the image, then we compute an inflated destination rect that will hold the entire image
    // and then set a clip to the portion that we want to display.
    FloatRect adjustedDestRect = destRect;

    if (srcRect.size() != imageSize) {
        // FIXME: Implement image scaling
        notImplemented();
    }

    // If the image is only partially loaded, then shrink the destination rect that we're drawing into accordingly.
    if (!shouldUseSubimage && currHeight < imageSize.height())
        adjustedDestRect.setHeight(adjustedDestRect.height() * currHeight / imageSize.height());

    setPlatformCompositeOperation(op, blendMode);

    // ImageOrientation expects the origin to be at (0, 0).
    transform.translate(adjustedDestRect.x(), adjustedDestRect.y());
    context->SetTransform(transform);
    adjustedDestRect.setLocation(FloatPoint());

    if (orientation != DefaultImageOrientation) {
        this->concatCTM(orientation.transformFromDefault(adjustedDestRect.size()));
        if (orientation.usesWidthAsHeight()) {
            // The destination rect will have it's width and height already reversed for the orientation of
            // the image, as it was needed for page layout, so we need to reverse it back here.
            adjustedDestRect = FloatRect(adjustedDestRect.x(), adjustedDestRect.y(), adjustedDestRect.height(), adjustedDestRect.width());
        }
    }

    context->SetTags(1, __LINE__);

    drawWithoutShadow(adjustedDestRect, [this, image, adjustedDestRect, srcRect](ID2D1RenderTarget* renderTarget) {
        renderTarget->DrawBitmap(image.get(), adjustedDestRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, static_cast<D2D1_RECT_F>(srcRect));
    });

    if (!stateSaver.didSave())
        context->SetTransform(ctm);
}

void GraphicsContext::releaseWindowsContext(HDC hdc, const IntRect& dstRect, bool supportAlphaBlend, bool mayCreateBitmap)
{
}

void GraphicsContext::drawWindowsBitmap(WindowsBitmap* image, const IntPoint& point)
{
}

void GraphicsContext::drawFocusRing(const Path& path, float width, float offset, const Color& color)
{
}

void GraphicsContext::drawFocusRing(const Vector<FloatRect>& rects, float width, float offset, const Color& color)
{
}

void GraphicsContext::updateDocumentMarkerResources()
{
}

void GraphicsContext::drawLineForDocumentMarker(const FloatPoint& point, float width, DocumentMarkerLineStyle style)
{
}

COMPtr<ID2D1SolidColorBrush> GraphicsContextPlatformPrivate::brushWithColor(const D2D1_COLOR_F& color)
{
    RGBA32 colorKey = makeRGBA32FromFloats(color.r, color.g, color.b, color.a);

    if (!colorKey) {
        if (!m_zeroBrush)
            m_renderTarget->CreateSolidColorBrush(color, &m_zeroBrush);
        return m_zeroBrush;
    }

    if (colorKey == 0xFFFFFFFF) {
        if (!m_whiteBrush)
            m_renderTarget->CreateSolidColorBrush(color, &m_whiteBrush);
        return m_whiteBrush;
    }

    auto existingBrush = m_solidColoredBrushCache.ensure(colorKey, [this, color] {
        ID2D1SolidColorBrush* colorBrush;
        m_renderTarget->CreateSolidColorBrush(color, &colorBrush);
        return colorBrush;
    });

    return existingBrush.iterator->value;
}

void GraphicsContextPlatformPrivate::clip(const FloatRect& rect)
{
    if (m_renderStates.isEmpty())
        save();

    m_renderTarget->PushAxisAlignedClip(rect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    m_renderStates.last().m_clips.append(GraphicsContextPlatformPrivate::AxisAlignedClip);
}

void GraphicsContextPlatformPrivate::clip(const Path& path)
{
    clip(path.platformPath());
}

void GraphicsContextPlatformPrivate::clip(ID2D1Geometry* path)
{
    ASSERT(m_renderStates.size());
    if (!m_renderStates.size())
        return;

    COMPtr<ID2D1Layer> clipLayer;
    HRESULT hr = m_renderTarget->CreateLayer(&clipLayer);
    ASSERT(SUCCEEDED(hr));
    if (!SUCCEEDED(hr))
        return;

    m_renderTarget->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), path), clipLayer.get());
    m_renderStates.last().m_clips.append(GraphicsContextPlatformPrivate::LayerClip);
    m_renderStates.last().m_activeLayer = clipLayer;
}

void GraphicsContextPlatformPrivate::concatCTM(const AffineTransform& affineTransform)
{
    ASSERT(m_renderTarget.get());

    D2D1_MATRIX_3X2_F currentTransform;
    m_renderTarget->GetTransform(&currentTransform);

    m_renderTarget->SetTransform(affineTransform * AffineTransform(currentTransform));
}

void GraphicsContextPlatformPrivate::flush()
{
    ASSERT(m_renderTarget.get());
    D2D1_TAG first, second;
    HRESULT hr = m_renderTarget->Flush(&first, &second);

    RELEASE_ASSERT(SUCCEEDED(hr));
}

void GraphicsContextPlatformPrivate::endDraw()
{
    ASSERT(m_renderTarget.get());
    D2D1_TAG first, second;
    HRESULT hr = m_renderTarget->EndDraw(&first, &second);

    RELEASE_ASSERT(SUCCEEDED(hr));
}

void GraphicsContextPlatformPrivate::restore()
{
    ASSERT(m_renderTarget.get());

    auto restoreState = m_renderStates.takeLast();
    m_renderTarget->RestoreDrawingState(restoreState.m_drawingStateBlock.get());

    for (auto clipType = restoreState.m_clips.rbegin(); clipType != restoreState.m_clips.rend(); ++clipType) {
        if (*clipType == GraphicsContextPlatformPrivate::AxisAlignedClip)
            m_renderTarget->PopAxisAlignedClip();
        else
            m_renderTarget->PopLayer();
    }
}

void GraphicsContextPlatformPrivate::save()
{
    ASSERT(m_renderTarget.get());

    RenderState currentState;
    GraphicsContext::systemFactory()->CreateDrawingStateBlock(&currentState.m_drawingStateBlock);

    m_renderTarget->SaveDrawingState(currentState.m_drawingStateBlock.get());

    m_renderStates.append(currentState);
}

void GraphicsContextPlatformPrivate::scale(const FloatSize& size)
{
    ASSERT(m_renderTarget.get());

    D2D1_MATRIX_3X2_F currentTransform;
    m_renderTarget->GetTransform(&currentTransform);

    auto scale = D2D1::Matrix3x2F::Scale(size);
    m_renderTarget->SetTransform(scale * currentTransform);
}

void GraphicsContextPlatformPrivate::setCTM(const AffineTransform& transform)
{
    ASSERT(m_renderTarget.get());
    m_renderTarget->SetTransform(transform);
}

void GraphicsContextPlatformPrivate::translate(float x, float y)
{
    ASSERT(m_renderTarget.get());

    D2D1_MATRIX_3X2_F currentTransform;
    m_renderTarget->GetTransform(&currentTransform);

    auto translation = D2D1::Matrix3x2F::Translation(x, y);
    m_renderTarget->SetTransform(translation * currentTransform);
}

void GraphicsContextPlatformPrivate::rotate(float angle)
{
    ASSERT(m_renderTarget.get());

    D2D1_MATRIX_3X2_F currentTransform;
    m_renderTarget->GetTransform(&currentTransform);

    auto rotation = D2D1::Matrix3x2F::Rotation(rad2deg(angle));
    m_renderTarget->SetTransform(rotation * currentTransform);
}

D2D1_COLOR_F GraphicsContext::colorWithGlobalAlpha(const Color& color) const
{
    float colorAlpha = color.alpha() / 255.0f;
    float globalAlpha = m_state.alpha;

    return D2D1::ColorF(color.rgb(), globalAlpha * colorAlpha);
}

ID2D1SolidColorBrush* GraphicsContext::solidStrokeBrush()
{
    return m_data->m_solidStrokeBrush.get();
}

ID2D1SolidColorBrush* GraphicsContext::solidFillBrush()
{
    return m_data->m_solidFillBrush.get();
}

void GraphicsContext::drawPattern(Image& image, const FloatRect& destRect, const FloatRect& tileRect, const AffineTransform& patternTransform, const FloatPoint& phase, const FloatSize& spacing, CompositeOperator op, BlendMode blendMode)
{
    if (paintingDisabled() || !patternTransform.isInvertible())
        return;

    if (isRecording()) {
        m_displayListRecorder->drawPattern(image, destRect, tileRect, patternTransform, phase, spacing, op, blendMode);
        return;
    }

    notImplemented();
}

void GraphicsContext::clipToImageBuffer(ImageBuffer& buffer, const FloatRect& destRect)
{
    if (paintingDisabled())
        return;

    FloatSize bufferDestinationSize = buffer.sizeForDestinationSize(destRect.size());
    notImplemented();
}

// Draws a filled rectangle with a stroked border.
void GraphicsContext::drawRect(const FloatRect& rect, float borderThickness)
{
    if (paintingDisabled())
        return;

    if (isRecording()) {
        m_displayListRecorder->drawRect(rect, borderThickness);
        return;
    }

    // FIXME: this function does not handle patterns and gradients like drawPath does, it probably should.
    ASSERT(!rect.isEmpty());

    auto context = platformContext();

    context->SetTags(1, __LINE__);

    drawWithoutShadow(rect, [this, rect](ID2D1RenderTarget* renderTarget) {
        const D2D1_RECT_F d2dRect = rect;
        renderTarget->DrawRectangle(&d2dRect, solidStrokeBrush(), strokeThickness());
        renderTarget->FillRectangle(&d2dRect, solidFillBrush());
    });
}

// This is only used to draw borders.
void GraphicsContext::drawLine(const FloatPoint& point1, const FloatPoint& point2)
{
    if (paintingDisabled())
        return;

    if (strokeStyle() == NoStroke)
        return;

    if (isRecording()) {
        m_displayListRecorder->drawLine(point1, point2);
        return;
    }

    float thickness = strokeThickness();
    bool isVerticalLine = (point1.x() + thickness == point2.x());
    float strokeWidth = isVerticalLine ? point2.y() - point1.y() : point2.x() - point1.x();
    if (!thickness || !strokeWidth)
        return;

    auto context = platformContext();

    StrokeStyle strokeStyle = this->strokeStyle();
    float cornerWidth = 0;
    bool drawsDashedLine = strokeStyle == DottedStroke || strokeStyle == DashedStroke;

    COMPtr<ID2D1StrokeStyle> d2dStrokeStyle;
    D2DContextStateSaver stateSaver(*m_data, drawsDashedLine);
    if (drawsDashedLine) {
        // Figure out end points to ensure we always paint corners.
        cornerWidth = strokeStyle == DottedStroke ? thickness : std::min(2 * thickness, std::max(thickness, strokeWidth / 3));
        strokeWidth -= 2 * cornerWidth;
        float patternWidth = strokeStyle == DottedStroke ? thickness : std::min(3 * thickness, std::max(thickness, strokeWidth / 3));
        // Check if corner drawing sufficiently covers the line.
        if (strokeWidth <= patternWidth + 1)
            return;

        // Pattern starts with full fill and ends with the empty fill.
        // 1. Let's start with the empty phase after the corner.
        // 2. Check if we've got odd or even number of patterns and whether they fully cover the line.
        // 3. In case of even number of patterns and/or remainder, move the pattern start position
        // so that the pattern is balanced between the corners.
        float patternOffset = patternWidth;
        int numberOfSegments = floorf(strokeWidth / patternWidth);
        bool oddNumberOfSegments = numberOfSegments % 2;
        float remainder = strokeWidth - (numberOfSegments * patternWidth);
        if (oddNumberOfSegments && remainder)
            patternOffset -= remainder / 2;
        else if (!oddNumberOfSegments) {
            if (remainder)
                patternOffset += patternOffset - (patternWidth + remainder) / 2;
            else
                patternOffset += patternWidth / 2;
        }
        const float dashes[2] = { patternWidth, patternWidth };

        auto strokeStyleProperties = D2D1::StrokeStyleProperties();

        GraphicsContext::systemFactory()->CreateStrokeStyle(&strokeStyleProperties, dashes, ARRAYSIZE(dashes), &d2dStrokeStyle);
    }

    FloatPoint p1 = point1;
    FloatPoint p2 = point2;
    // Center line and cut off corners for pattern patining.
    if (isVerticalLine) {
        float centerOffset = (p2.x() - p1.x()) / 2;
        p1.move(centerOffset, cornerWidth);
        p2.move(-centerOffset, -cornerWidth);
    } else {
        float centerOffset = (p2.y() - p1.y()) / 2;
        p1.move(cornerWidth, centerOffset);
        p2.move(-cornerWidth, -centerOffset);
    }

    context->SetTags(1, __LINE__);

    FloatRect boundingRect(p1, p2);

    drawWithoutShadow(boundingRect, [this, p1, p2, d2dStrokeStyle](ID2D1RenderTarget* renderTarget) {
        renderTarget->DrawLine(p1, p2, solidStrokeBrush(), strokeThickness(), d2dStrokeStyle.get());
    });
}

void GraphicsContext::drawEllipse(const FloatRect& rect)
{
    if (paintingDisabled())
        return;

    if (isRecording()) {
        m_displayListRecorder->drawEllipse(rect);
        return;
    }

    auto ellipse = D2D1::Ellipse(D2D1::Point2F(rect.x(), rect.y()), rect.width(), rect.height());

    auto context = platformContext();

    context->SetTags(1, __LINE__);

    drawWithoutShadow(rect, [this, ellipse](ID2D1RenderTarget* renderTarget) {
        renderTarget->FillEllipse(&ellipse, solidFillBrush());

        renderTarget->DrawEllipse(&ellipse, solidStrokeBrush(), strokeThickness());
    });
}

void GraphicsContext::applyStrokePattern()
{
    if (paintingDisabled())
        return;

    // Note: Because of the way Direct2D draws, we may not need this explicit context 'set stroke pattern' logic.
    notImplemented();
}

void GraphicsContext::applyFillPattern()
{
    if (paintingDisabled())
        return;

    // Note: Because of the way Direct2D draws, we may not need this explicit context 'set fill pattern' logic.
    notImplemented();
}

void GraphicsContext::drawPath(const Path& path)
{
    if (paintingDisabled() || path.isEmpty())
        return;

    if (isRecording()) {
        m_displayListRecorder->drawPath(path);
        return;
    }

    auto context = platformContext();
    const GraphicsContextState& state = m_state;

    if (state.fillGradient || state.strokeGradient) {
        // We don't have any optimized way to fill & stroke a path using gradients
        // FIXME: Be smarter about this.
        fillPath(path);
        strokePath(path);
        return;
    }

    if (state.fillPattern)
        applyFillPattern();

    if (state.strokePattern)
        applyStrokePattern();

    if (path.activePath())
        path.activePath()->Close();

    context->SetTags(1, __LINE__);

    auto rect = path.fastBoundingRect();
    drawWithoutShadow(rect, [this, &path](ID2D1RenderTarget* renderTarget) {
        renderTarget->DrawGeometry(path.platformPath(), solidStrokeBrush());
    });
}

void GraphicsContext::drawWithoutShadow(const FloatRect& /*boundingRect*/, const std::function<void(ID2D1RenderTarget*)>& drawCommands)
{
    auto context = platformContext();

    if (!didBeginDraw())
        context->BeginDraw();

    drawCommands(context);

    if (!didBeginDraw())
        m_data->endDraw();
}

void GraphicsContext::drawWithShadow(const FloatRect& boundingRect, const std::function<void(ID2D1RenderTarget*)>& drawCommands)
{
    auto context = platformContext();

    // Render the current geometry to a bitmap context
    COMPtr<ID2D1BitmapRenderTarget> bitmapTarget;
    HRESULT hr = context->CreateCompatibleRenderTarget(&bitmapTarget);
    RELEASE_ASSERT(SUCCEEDED(hr));

    bitmapTarget->BeginDraw();
    drawCommands(bitmapTarget.get());
    hr = bitmapTarget->EndDraw();
    RELEASE_ASSERT(SUCCEEDED(hr));

    COMPtr<ID2D1Bitmap> bitmap;
    hr = bitmapTarget->GetBitmap(&bitmap);
    RELEASE_ASSERT(SUCCEEDED(hr));

    COMPtr<ID2D1DeviceContext> deviceContext;
    hr = context->QueryInterface(&deviceContext);
    RELEASE_ASSERT(SUCCEEDED(hr));

    // Create the shadow effect
    COMPtr<ID2D1Effect> shadowEffect;
    hr = deviceContext->CreateEffect(CLSID_D2D1Shadow, &shadowEffect);
    RELEASE_ASSERT(SUCCEEDED(hr));

    shadowEffect->SetInput(0, bitmap.get());
    shadowEffect->SetValue(D2D1_SHADOW_PROP_COLOR, static_cast<D2D1_VECTOR_4F>(m_state.shadowColor));
    shadowEffect->SetValue(D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION, m_state.shadowBlur);

    COMPtr<ID2D1Effect> transformEffect;
    hr = deviceContext->CreateEffect(CLSID_D2D12DAffineTransform, &transformEffect);
    RELEASE_ASSERT(SUCCEEDED(hr));

    transformEffect->SetInputEffect(0, shadowEffect.get());

    auto translation = D2D1::Matrix3x2F::Translation(m_state.shadowOffset.width(), m_state.shadowOffset.height());
    transformEffect->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, translation);

    COMPtr<ID2D1Effect> compositor;
    hr = deviceContext->CreateEffect(CLSID_D2D1Composite, &compositor);
    RELEASE_ASSERT(SUCCEEDED(hr));

    compositor->SetInputEffect(0, transformEffect.get());
    compositor->SetInput(1, bitmap.get());

    // Flip the context
    D2D1_MATRIX_3X2_F ctm;
    deviceContext->GetTransform(&ctm);
    auto translate = D2D1::Matrix3x2F::Translation(0.0f, deviceContext->GetSize().height);
    auto flip = D2D1::Matrix3x2F::Scale(D2D1::SizeF(1.0f, -1.0f));
    deviceContext->SetTransform(ctm * flip * translate);

    deviceContext->BeginDraw();
    deviceContext->DrawImage(compositor.get(), D2D1_INTERPOLATION_MODE_LINEAR);
    deviceContext->EndDraw();
}

void GraphicsContext::fillPath(const Path& path)
{
    if (paintingDisabled() || path.isEmpty())
        return;

    if (isRecording()) {
        m_displayListRecorder->fillPath(path);
        return;
    }

    if (path.activePath()) {
        // Make sure it's closed. This might fail if the path was already closed, so
        // ignore the return value.
        path.activePath()->Close();
    }

    D2DContextStateSaver stateSaver(*m_data);

    auto context = platformContext();

    context->SetTags(1, __LINE__);

    if (m_state.fillGradient) {
        context->SetTags(1, __LINE__);

        FloatRect boundingRect = path.fastBoundingRect();
        auto drawFunction = [this, &path](ID2D1RenderTarget* renderTarget) {
            renderTarget->FillGeometry(path.platformPath(), m_state.fillGradient->createPlatformGradientIfNecessary(renderTarget));
        };

        if (hasShadow())
            drawWithShadow(boundingRect, drawFunction);
        else
            drawWithoutShadow(boundingRect, drawFunction);
        return;
    }

    if (m_state.fillPattern)
        applyFillPattern();

    COMPtr<ID2D1GeometryGroup> pathToFill;
    path.createGeometryWithFillMode(fillRule(), pathToFill);

    context->SetTags(1, __LINE__);

    FloatRect contextRect(FloatPoint(), context->GetSize());
    drawWithoutShadow(contextRect, [this, &pathToFill](ID2D1RenderTarget* renderTarget) {
        renderTarget->FillGeometry(pathToFill.get(), solidFillBrush());
    });
}

void GraphicsContext::strokePath(const Path& path)
{
    if (paintingDisabled() || path.isEmpty())
        return;

    if (isRecording()) {
        m_displayListRecorder->strokePath(path);
        return;
    }

    auto context = platformContext();
    
    context->SetTags(1, __LINE__);

    if (m_state.strokeGradient) {
        context->SetTags(1, __LINE__);

        D2DContextStateSaver stateSaver(*m_data);
        auto boundingRect = path.fastBoundingRect();
        auto drawFunction = [this, &path](ID2D1RenderTarget* renderTarget) {
            renderTarget->DrawGeometry(path.platformPath(), m_state.strokeGradient->createPlatformGradientIfNecessary(renderTarget));
        };

        if (hasShadow())
            drawWithShadow(boundingRect, drawFunction);
        else
            drawWithoutShadow(boundingRect, drawFunction);

        return;
    }

    if (m_state.strokePattern)
        applyStrokePattern();

    context->SetTags(1, __LINE__);

    FloatRect contextRect(FloatPoint(), context->GetSize());
    drawWithoutShadow(contextRect, [this, &path](ID2D1RenderTarget* renderTarget) {
        renderTarget->DrawGeometry(path.platformPath(), solidStrokeBrush(), strokeThickness());
    });
}

void GraphicsContext::fillRect(const FloatRect& rect)
{
    if (paintingDisabled())
        return;

    if (isRecording()) {
        m_displayListRecorder->fillRect(rect);
        return;
    }

    auto context = platformContext();

    if (m_state.fillGradient) {
        context->SetTags(1, __LINE__);
        D2DContextStateSaver stateSaver(*m_data);
        auto drawFunction = [this, rect](ID2D1RenderTarget* renderTarget) {
            const D2D1_RECT_F d2dRect = rect;
            renderTarget->FillRectangle(&d2dRect, m_state.fillGradient->createPlatformGradientIfNecessary(renderTarget));
        };

        if (hasShadow())
            drawWithShadow(rect, drawFunction);
        else
            drawWithoutShadow(rect, drawFunction);
        return;
    }

    if (m_state.fillPattern)
        applyFillPattern();

    context->SetTags(1, __LINE__);

    bool drawOwnShadow = !isAcceleratedContext() && hasBlurredShadow() && !m_state.shadowsIgnoreTransforms; // Don't use ShadowBlur for canvas yet.
    if (drawOwnShadow) {
        // FIXME: Get ShadowBlur working on Direct2D
        // ShadowBlur contextShadow(m_state);
        // contextShadow.drawRectShadow(*this, FloatRoundedRect(rect));
        notImplemented();
    }

    drawWithoutShadow(rect, [this, rect](ID2D1RenderTarget* renderTarget) {
        const D2D1_RECT_F d2dRect = rect;
        renderTarget->FillRectangle(&d2dRect, solidFillBrush());
    });
}

void GraphicsContext::fillRect(const FloatRect& rect, const Color& color)
{
    if (paintingDisabled())
        return;

    if (isRecording()) {
        m_displayListRecorder->fillRect(rect, color);
        return;
    }

    auto context = platformContext();

    bool drawOwnShadow = !isAcceleratedContext() && hasBlurredShadow() && !m_state.shadowsIgnoreTransforms; // Don't use ShadowBlur for canvas yet.
    if (drawOwnShadow) {
        // FIXME: Get ShadowBlur working on Direct2D
        // ShadowBlur contextShadow(m_state);
        // contextShadow.drawRectShadow(*this, FloatRoundedRect(rect));
        notImplemented();
    }

    context->SetTags(1, __LINE__);

    drawWithoutShadow(rect, [this, rect, color](ID2D1RenderTarget* renderTarget) {
        const D2D1_RECT_F d2dRect = rect;
        renderTarget->FillRectangle(&d2dRect, m_data->brushWithColor(colorWithGlobalAlpha(color)).get());
    });
}

void GraphicsContext::platformFillRoundedRect(const FloatRoundedRect& rect, const Color& color)
{
    if (paintingDisabled())
        return;

    ASSERT(!isRecording());

    auto context = platformContext();

    bool drawOwnShadow = !isAcceleratedContext() && hasBlurredShadow() && !m_state.shadowsIgnoreTransforms; // Don't use ShadowBlur for canvas yet.
    D2DContextStateSaver stateSaver(*m_data, drawOwnShadow);
    if (drawOwnShadow) {
        // FIXME: Get ShadowBlur working on Direct2D
        // ShadowBlur contextShadow(m_state);
        // contextShadow.drawRectShadow(*this, rect);
        notImplemented();
    }

    if (!didBeginDraw())
        context->BeginDraw();
    
    context->SetTags(1, __LINE__);

    const FloatRect& r = rect.rect();
    const FloatRoundedRect::Radii& radii = rect.radii();
    bool equalWidths = (radii.topLeft().width() == radii.topRight().width() && radii.topRight().width() == radii.bottomLeft().width() && radii.bottomLeft().width() == radii.bottomRight().width());
    bool equalHeights = (radii.topLeft().height() == radii.bottomLeft().height() && radii.bottomLeft().height() == radii.topRight().height() && radii.topRight().height() == radii.bottomRight().height());
    bool hasCustomFill = m_state.fillGradient || m_state.fillPattern;
    if (!hasCustomFill && equalWidths && equalHeights && radii.topLeft().width() * 2 == r.width() && radii.topLeft().height() * 2 == r.height()) {
        auto roundedRect = D2D1::RoundedRect(r, radii.topLeft().width(), radii.topLeft().height());
        COMPtr<ID2D1SolidColorBrush> fillBrush = m_data->brushWithColor(colorWithGlobalAlpha(color));
        context->FillRoundedRectangle(roundedRect, fillBrush.get());
    } else {
        D2DContextStateSaver stateSaver(*m_data);
        setFillColor(color);

        Path path;
        path.addRoundedRect(rect);
        fillPath(path);
    }

    if (!didBeginDraw())
        m_data->endDraw();

    if (drawOwnShadow)
        stateSaver.restore();
}

void GraphicsContext::fillRectWithRoundedHole(const FloatRect& rect, const FloatRoundedRect& roundedHoleRect, const Color& color)
{
    if (paintingDisabled())
        return;

    if (isRecording()) {
        m_displayListRecorder->fillRectWithRoundedHole(rect, roundedHoleRect, color);
        return;
    }

    auto context = platformContext();

    if (!didBeginDraw())
        context->BeginDraw();

    context->SetTags(1, __LINE__);

    Path path;
    path.addRect(rect);

    if (!roundedHoleRect.radii().isZero())
        path.addRoundedRect(roundedHoleRect);
    else
        path.addRect(roundedHoleRect.rect());

    WindRule oldFillRule = fillRule();
    Color oldFillColor = fillColor();

    setFillRule(RULE_EVENODD);
    setFillColor(color);

    // fillRectWithRoundedHole() assumes that the edges of rect are clipped out, so we only care about shadows cast around inside the hole.
    bool drawOwnShadow = !isAcceleratedContext() && hasBlurredShadow() && !m_state.shadowsIgnoreTransforms;
    D2DContextStateSaver stateSaver(*m_data, drawOwnShadow);
    if (drawOwnShadow) {
        // FIXME: Get ShadowBlur working on Direct2D
        // ShadowBlur contextShadow(m_state);
        // contextShadow.drawRectShadow(*this, rect);
        notImplemented();
    }

    fillPath(path);

    if (!didBeginDraw())
        m_data->endDraw();

    if (drawOwnShadow)
        stateSaver.restore();

    setFillRule(oldFillRule);
    setFillColor(oldFillColor);
}

void GraphicsContext::clip(const FloatRect& rect)
{
    if (paintingDisabled())
        return;

    if (isRecording()) {
        m_displayListRecorder->clip(rect);
        return;
    }

    m_data->clip(rect);
}

void GraphicsContext::clipOut(const FloatRect& rect)
{
    if (paintingDisabled())
        return;

    if (isRecording()) {
        m_displayListRecorder->clipOut(rect);
        return;
    }

    Path path;
    path.addRect(rect);

    clipOut(path);
}

void GraphicsContext::clipOut(const Path& path)
{
    if (paintingDisabled())
        return;

    if (isRecording()) {
        m_displayListRecorder->clipOut(path);
        return;
    }

    // To clip Out we need the intersection of the infinite
    // clipping rect and the path we just created.
    D2D1_SIZE_F rendererSize = platformContext()->GetSize();
    FloatRect clipBounds(0, 0, rendererSize.width, rendererSize.height);

    Path boundingRect;
    boundingRect.addRect(clipBounds);
    boundingRect.appendGeometry(path.platformPath());

    COMPtr<ID2D1GeometryGroup> pathToClip;
    boundingRect.createGeometryWithFillMode(RULE_EVENODD, pathToClip);

    m_data->clip(pathToClip.get());
}

void GraphicsContext::clipPath(const Path& path, WindRule clipRule)
{
    if (paintingDisabled())
        return;

    if (isRecording()) {
        m_displayListRecorder->clipPath(path, clipRule);
        return;
    }

    auto context = platformContext();
    if (path.isEmpty()) {
        m_data->clip(FloatRect());
        return;
    }

    COMPtr<ID2D1GeometryGroup> pathToClip;
    path.createGeometryWithFillMode(clipRule, pathToClip);

    m_data->clip(pathToClip.get());
}

IntRect GraphicsContext::clipBounds() const
{
    if (paintingDisabled())
        return IntRect();

    if (isRecording()) {
        WTFLogAlways("Getting the clip bounds not yet supported with display lists");
        return IntRect(-2048, -2048, 4096, 4096); // FIXME: display lists.
    }

    D2D1_RECT_F d2dClipBounds = D2D1::InfiniteRect();
    FloatRect clipBounds(d2dClipBounds.left, d2dClipBounds.top, d2dClipBounds.right - d2dClipBounds.left, d2dClipBounds.bottom - d2dClipBounds.top);

    if (m_data->clipLayer()) {
        auto clipSize = m_data->clipLayer()->GetSize();
        clipBounds.setWidth(clipSize.width);
        clipBounds.setHeight(clipSize.height);
    }

    return enclosingIntRect(clipBounds);
}

void GraphicsContext::beginPlatformTransparencyLayer(float opacity)
{
    if (paintingDisabled())
        return;

    ASSERT(!isRecording());

    save();

    notImplemented();
}

void GraphicsContext::endPlatformTransparencyLayer()
{
    if (paintingDisabled())
        return;

    ASSERT(!isRecording());

    notImplemented();

    restore();
}

bool GraphicsContext::supportsTransparencyLayers()
{
    return false;
}

void GraphicsContext::setPlatformShadow(const FloatSize& offset, float blur, const Color& color)
{
    (void)offset;
    (void)blur;
    (void)color;
    notImplemented();
}

void GraphicsContext::clearPlatformShadow()
{
    if (paintingDisabled())
        return;
    notImplemented();
}

void GraphicsContext::setMiterLimit(float limit)
{
    if (paintingDisabled())
        return;

    if (isRecording()) {
        // Maybe this should be part of the state.
        m_displayListRecorder->setMiterLimit(limit);
        return;
    }

    notImplemented();
}

void GraphicsContext::clearRect(const FloatRect& rect)
{
    if (paintingDisabled())
        return;

    if (isRecording()) {
        m_displayListRecorder->clearRect(rect);
        return;
    }

    auto context = platformContext();

    context->SetTags(1, __LINE__);

    drawWithoutShadow(rect, [this, rect](ID2D1RenderTarget* renderTarget) {
        FloatSize renderTargetSize = renderTarget->GetSize();
        if (rect.size() == renderTargetSize)
            renderTarget->Clear();
        else
            renderTarget->FillRectangle(rect, m_data->brushWithColor(colorWithGlobalAlpha(fillColor())).get());
    });
}

void GraphicsContext::strokeRect(const FloatRect& rect, float lineWidth)
{
    (void)rect;
    (void)lineWidth;
    notImplemented();
}

void GraphicsContext::setLineCap(LineCap cap)
{
    (void)cap;
    notImplemented();
}

void GraphicsContext::setLineDash(const DashArray& dashes, float dashOffset)
{
    if (paintingDisabled())
        return;

    if (isRecording()) {
        m_displayListRecorder->setLineDash(dashes, dashOffset);
        return;
    }

    if (dashOffset < 0) {
        float length = 0;
        for (size_t i = 0; i < dashes.size(); ++i)
            length += static_cast<float>(dashes[i]);
        if (length)
            dashOffset = fmod(dashOffset, length) + length;
    }

    notImplemented();
}

void GraphicsContext::setLineJoin(LineJoin join)
{
    if (paintingDisabled())
        return;

    if (isRecording()) {
        m_displayListRecorder->setLineJoin(join);
        return;
    }

    notImplemented();
}

void GraphicsContext::canvasClip(const Path& path, WindRule fillRule)
{
    clipPath(path, fillRule);
}

void GraphicsContext::scale(const FloatSize& size)
{
    if (paintingDisabled())
        return;

    if (isRecording()) {
        m_displayListRecorder->scale(size);
        return;
    }

    m_data->scale(size);
    // FIXME: m_data->m_userToDeviceTransformKnownToBeIdentity = false;
}

void GraphicsContext::rotate(float angle)
{
    if (paintingDisabled())
        return;

    if (isRecording()) {
        m_displayListRecorder->rotate(angle);
        return;
    }

    m_data->rotate(angle);
    // FIXME: m_data->m_userToDeviceTransformKnownToBeIdentity = false;
}

void GraphicsContext::translate(float x, float y)
{
    if (paintingDisabled())
        return;

    if (isRecording()) {
        m_displayListRecorder->translate(x, y);
        return;
    }

    m_data->translate(x, y);
    // FIXME: m_data->m_userToDeviceTransformKnownToBeIdentity = false;
}

void GraphicsContext::concatCTM(const AffineTransform& transform)
{
    if (paintingDisabled())
        return;

    if (isRecording()) {
        m_displayListRecorder->concatCTM(transform);
        return;
    }

    m_data->concatCTM(transform);
    // FIXME: m_data->m_userToDeviceTransformKnownToBeIdentity = false;
}

void GraphicsContext::setCTM(const AffineTransform& transform)
{
    if (paintingDisabled())
        return;

    if (isRecording()) {
        WTFLogAlways("GraphicsContext::setCTM() is not compatible with recording contexts.");
        return;
    }

    m_data->setCTM(transform);
    // FIXME: m_data->m_userToDeviceTransformKnownToBeIdentity = false;
}

AffineTransform GraphicsContext::getCTM(IncludeDeviceScale includeScale) const
{
    if (paintingDisabled())
        return AffineTransform();

    if (isRecording()) {
        WTFLogAlways("GraphicsContext::getCTM() is not yet compatible with recording contexts.");
        return AffineTransform();
    }

    D2D1_MATRIX_3X2_F currentTransform;
    platformContext()->GetTransform(&currentTransform);
    return currentTransform;
}

FloatRect GraphicsContext::roundToDevicePixels(const FloatRect& rect, RoundingMode roundingMode)
{
    if (paintingDisabled())
        return rect;

    if (isRecording()) {
        WTFLogAlways("GraphicsContext::roundToDevicePixels() is not yet compatible with recording contexts.");
        return rect;
    }

    notImplemented();

    return rect;
}

void GraphicsContext::drawLineForText(const FloatPoint& point, float width, bool printing, bool doubleLines, StrokeStyle strokeStyle)
{
    DashArray widths;
    widths.append(width);
    widths.append(0);
    drawLinesForText(point, widths, printing, doubleLines, strokeStyle);
}

void GraphicsContext::drawLinesForText(const FloatPoint& point, const DashArray& widths, bool printing, bool doubleLines, StrokeStyle strokeStyle)
{
    if (paintingDisabled())
        return;

    if (!widths.size())
        return;

    if (isRecording()) {
        m_displayListRecorder->drawLinesForText(point, widths, printing, doubleLines, strokeThickness());
        return;
    }

    notImplemented();
}

void GraphicsContext::setURLForRect(const URL& link, const IntRect& destRect)
{
    if (paintingDisabled())
        return;

    if (isRecording()) {
        WTFLogAlways("GraphicsContext::setURLForRect() is not yet compatible with recording contexts.");
        return; // FIXME for display lists.
    }

    RetainPtr<CFURLRef> urlRef = link.createCFURL();
    if (!urlRef)
        return;

    notImplemented();
}

void GraphicsContext::setPlatformImageInterpolationQuality(InterpolationQuality mode)
{
    ASSERT(!paintingDisabled());

    D2D1_INTERPOLATION_MODE quality = D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR;

    switch (mode) {
    case InterpolationDefault:
        quality = D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR;
        break;
    case InterpolationNone:
        quality = D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR;
        break;
    case InterpolationLow:
        quality = D2D1_INTERPOLATION_MODE_LINEAR;
        break;
    case InterpolationMedium:
        quality = D2D1_INTERPOLATION_MODE_CUBIC;
        break;
    case InterpolationHigh:
        quality = D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC;
        break;
    }
    // FIXME: SetInterpolationQuality(platformContext(), quality);
}

void GraphicsContext::setIsCALayerContext(bool isLayerContext)
{
    if (paintingDisabled())
        return;

    if (isRecording())
        return;

    // This function is probabaly not needed.
    notImplemented();
}

bool GraphicsContext::isCALayerContext() const
{
    if (paintingDisabled())
        return false;

    // FIXME
    if (isRecording())
        return false;

    // This function is probabaly not needed.
    notImplemented();
    return false;
}

void GraphicsContext::setIsAcceleratedContext(bool isAccelerated)
{
    if (paintingDisabled())
        return;

    // FIXME
    if (isRecording())
        return;

    notImplemented();
}

bool GraphicsContext::isAcceleratedContext() const
{
    if (paintingDisabled())
        return false;

    // FIXME
    if (isRecording())
        return false;

    // This function is probabaly not needed.
    notImplemented();
    return false;
}

void GraphicsContext::setPlatformTextDrawingMode(TextDrawingModeFlags mode)
{
    (void)mode;
    notImplemented();
}

void GraphicsContext::setPlatformStrokeColor(const Color& color)
{
    ASSERT(m_state.strokeColor == color);

    m_data->m_solidStrokeBrush = nullptr;

    m_data->m_solidStrokeBrush = m_data->brushWithColor(colorWithGlobalAlpha(strokeColor()));
}

void GraphicsContext::setPlatformStrokeThickness(float thickness)
{
    // This is a no-op on Windows. We fill using the GraphicsContextState::strokeThickness member.
    ASSERT(m_state.strokeThickness == thickness);
}

void GraphicsContext::setPlatformFillColor(const Color& color)
{
    ASSERT(m_state.fillColor == color);

    m_data->m_solidFillBrush = nullptr;

    m_data->m_solidFillBrush = m_data->brushWithColor(colorWithGlobalAlpha(fillColor()));
}

void GraphicsContext::setPlatformShouldAntialias(bool enable)
{
    if (paintingDisabled())
        return;

    ASSERT(!isRecording());

    auto antialiasMode = enable ? D2D1_ANTIALIAS_MODE_PER_PRIMITIVE : D2D1_ANTIALIAS_MODE_ALIASED;
    platformContext()->SetAntialiasMode(antialiasMode);
}

void GraphicsContext::setPlatformShouldSmoothFonts(bool enable)
{
    if (paintingDisabled())
        return;

    ASSERT(!isRecording());

    auto fontSmoothingMode = enable ? D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE : D2D1_TEXT_ANTIALIAS_MODE_ALIASED;
    platformContext()->SetTextAntialiasMode(fontSmoothingMode);
}

void GraphicsContext::setPlatformAlpha(float)
{
    /* No-op on this platform */
}

void GraphicsContext::setPlatformCompositeOperation(CompositeOperator mode, BlendMode blendMode)
{
    if (paintingDisabled())
        return;

    ASSERT(!isRecording());

    D2D1_BLEND_MODE targetBlendMode = D2D1_BLEND_MODE_SCREEN;
    D2D1_COMPOSITE_MODE targetCompositeMode = D2D1_COMPOSITE_MODE_SOURCE_ATOP; // ???

    if (blendMode != BlendModeNormal) {
        switch (blendMode) {
        case BlendModeMultiply:
            targetBlendMode = D2D1_BLEND_MODE_MULTIPLY;
            break;
        case BlendModeScreen:
            targetBlendMode = D2D1_BLEND_MODE_SCREEN;
            break;
        case BlendModeOverlay:
            targetBlendMode = D2D1_BLEND_MODE_OVERLAY;
            break;
        case BlendModeDarken:
            targetBlendMode = D2D1_BLEND_MODE_DARKEN;
            break;
        case BlendModeLighten:
            targetBlendMode = D2D1_BLEND_MODE_LIGHTEN;
            break;
        case BlendModeColorDodge:
            targetBlendMode = D2D1_BLEND_MODE_COLOR_DODGE;
            break;
        case BlendModeColorBurn:
            targetBlendMode = D2D1_BLEND_MODE_COLOR_BURN;
            break;
        case BlendModeHardLight:
            targetBlendMode = D2D1_BLEND_MODE_HARD_LIGHT;
            break;
        case BlendModeSoftLight:
            targetBlendMode = D2D1_BLEND_MODE_SOFT_LIGHT;
            break;
        case BlendModeDifference:
            targetBlendMode = D2D1_BLEND_MODE_DIFFERENCE;
            break;
        case BlendModeExclusion:
            targetBlendMode = D2D1_BLEND_MODE_EXCLUSION;
            break;
        case BlendModeHue:
            targetBlendMode = D2D1_BLEND_MODE_HUE;
            break;
        case BlendModeSaturation:
            targetBlendMode = D2D1_BLEND_MODE_SATURATION;
            break;
        case BlendModeColor:
            targetBlendMode = D2D1_BLEND_MODE_COLOR;
            break;
        case BlendModeLuminosity:
            targetBlendMode = D2D1_BLEND_MODE_LUMINOSITY;
            break;
        case BlendModePlusDarker:
            targetBlendMode = D2D1_BLEND_MODE_DARKER_COLOR;
            break;
        case BlendModePlusLighter:
            targetBlendMode = D2D1_BLEND_MODE_LIGHTER_COLOR;
            break;
        default:
            break;
        }
    } else {
        switch (mode) {
        case CompositeClear:
            // FIXME: targetBlendMode = D2D1_BLEND_MODE_CLEAR;
            break;
        case CompositeCopy:
            // FIXME: targetBlendMode = D2D1_BLEND_MODE_COPY;
            break;
        case CompositeSourceOver:
            // FIXME: kCGBlendModeNormal
            break;
        case CompositeSourceIn:
            targetCompositeMode = D2D1_COMPOSITE_MODE_SOURCE_IN;
            break;
        case CompositeSourceOut:
            targetCompositeMode = D2D1_COMPOSITE_MODE_SOURCE_OUT;
            break;
        case CompositeSourceAtop:
            targetCompositeMode = D2D1_COMPOSITE_MODE_SOURCE_ATOP;
            break;
        case CompositeDestinationOver:
            targetCompositeMode = D2D1_COMPOSITE_MODE_DESTINATION_OVER;
            break;
        case CompositeDestinationIn:
            targetCompositeMode = D2D1_COMPOSITE_MODE_DESTINATION_IN;
            break;
        case CompositeDestinationOut:
            targetCompositeMode = D2D1_COMPOSITE_MODE_DESTINATION_OUT;
            break;
        case CompositeDestinationAtop:
            targetCompositeMode = D2D1_COMPOSITE_MODE_DESTINATION_ATOP;
            break;
        case CompositeXOR:
            targetCompositeMode = D2D1_COMPOSITE_MODE_XOR;
            break;
        case CompositePlusDarker:
            targetBlendMode = D2D1_BLEND_MODE_DARKER_COLOR;
            break;
        case CompositePlusLighter:
            targetBlendMode = D2D1_BLEND_MODE_LIGHTER_COLOR;
            break;
        case CompositeDifference:
            targetBlendMode = D2D1_BLEND_MODE_DIFFERENCE;
            break;
        }
    }

    m_data->m_blendMode = targetBlendMode;
    m_data->m_compositeMode = targetCompositeMode;
}

void GraphicsContext::platformApplyDeviceScaleFactor(float deviceScaleFactor)
{
    // This is a no-op for Direct2D.
}

void GraphicsContext::platformFillEllipse(const FloatRect& ellipse)
{
    if (paintingDisabled())
        return;

    ASSERT(!isRecording());

    if (m_state.fillGradient || m_state.fillPattern) {
        // FIXME: We should be able to fill ellipses with pattern/gradient brushes in D2D.
        fillEllipseAsPath(ellipse);
        return;
    }

    auto d2dEllipse = D2D1::Ellipse(D2D1::Point2F(ellipse.x(), ellipse.y()), ellipse.width(), ellipse.height());

    platformContext()->SetTags(1, __LINE__);

    drawWithoutShadow(ellipse, [this, d2dEllipse](ID2D1RenderTarget* renderTarget) {
        renderTarget->FillEllipse(&d2dEllipse, solidFillBrush());
    });
}

void GraphicsContext::platformStrokeEllipse(const FloatRect& ellipse)
{
    if (paintingDisabled())
        return;

    ASSERT(!isRecording());

    if (m_state.strokeGradient || m_state.strokePattern) {
        // FIXME: We should be able to stroke ellipses with pattern/gradient brushes in D2D.
        strokeEllipseAsPath(ellipse);
        return;
    }

    auto d2dEllipse = D2D1::Ellipse(D2D1::Point2F(ellipse.x(), ellipse.y()), ellipse.width(), ellipse.height());

    platformContext()->SetTags(1, __LINE__);

    drawWithoutShadow(ellipse, [this, d2dEllipse](ID2D1RenderTarget* renderTarget) {
        renderTarget->DrawEllipse(&d2dEllipse, solidStrokeBrush(), strokeThickness());
    });
}

}
