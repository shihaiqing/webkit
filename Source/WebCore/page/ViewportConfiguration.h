/*
 * Copyright (C) 2005-2014 Apple Inc. All rights reserved.
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

#ifndef ViewportConfiguration_h
#define ViewportConfiguration_h

#include "FloatSize.h"
#include "IntSize.h"
#include "ViewportArguments.h"
#include <wtf/Noncopyable.h>

namespace WebCore {

static const double forceAlwaysUserScalableMaximumScale = 5.0;
static const double forceAlwaysUserScalableMinimumScale = 1.0;

class TextStream;

class ViewportConfiguration {
    WTF_MAKE_NONCOPYABLE(ViewportConfiguration); WTF_MAKE_FAST_ALLOCATED;
public:
    // FIXME: unify with ViewportArguments.
    struct Parameters {
        Parameters()
            : width(0)
            , height(0)
            , initialScale(0)
            , minimumScale(0)
            , maximumScale(0)
            , allowsUserScaling(false)
            , allowsShrinkToFit(false)
            , widthIsSet(false)
            , heightIsSet(false)
            , initialScaleIsSet(false)
        {
        }

        double width;
        double height;
        double initialScale;
        double minimumScale;
        double maximumScale;
        bool allowsUserScaling;
        bool allowsShrinkToFit;

        bool widthIsSet;
        bool heightIsSet;
        bool initialScaleIsSet;
    };

    WEBCORE_EXPORT ViewportConfiguration();

    const Parameters& defaultConfiguration() const { return m_defaultConfiguration; }
    WEBCORE_EXPORT void setDefaultConfiguration(const Parameters&);

    const IntSize& contentsSize() const { return m_contentSize; }
    WEBCORE_EXPORT bool setContentsSize(const IntSize&);

    const FloatSize& minimumLayoutSize() const { return m_minimumLayoutSize; }
    WEBCORE_EXPORT bool setMinimumLayoutSize(const FloatSize&);

    const ViewportArguments& viewportArguments() const { return m_viewportArguments; }
    WEBCORE_EXPORT bool setViewportArguments(const ViewportArguments&);

    WEBCORE_EXPORT bool setCanIgnoreScalingConstraints(bool);
    void setForceAlwaysUserScalable(bool forceAlwaysUserScalable) { m_forceAlwaysUserScalable = forceAlwaysUserScalable; }

    WEBCORE_EXPORT IntSize layoutSize() const;
    WEBCORE_EXPORT double initialScale() const;
    WEBCORE_EXPORT double initialScaleIgnoringContentSize() const;
    WEBCORE_EXPORT double minimumScale() const;
    double maximumScale() const { return m_forceAlwaysUserScalable ? forceAlwaysUserScalableMaximumScale : m_configuration.maximumScale; }
    double maximumScaleIgnoringAlwaysScalable() const { return m_configuration.maximumScale; }
    WEBCORE_EXPORT bool allowsUserScaling() const;
    WEBCORE_EXPORT bool allowsUserScalingIgnoringAlwaysScalable() const;
    bool allowsShrinkToFit() const;

    WEBCORE_EXPORT static Parameters webpageParameters();
    WEBCORE_EXPORT static Parameters textDocumentParameters();
    WEBCORE_EXPORT static Parameters imageDocumentParameters();
    WEBCORE_EXPORT static Parameters xhtmlMobileParameters();
    WEBCORE_EXPORT static Parameters testingParameters();
    
#ifndef NDEBUG
    WTF::CString description() const;
    WEBCORE_EXPORT void dump() const;
#endif

private:
    void updateConfiguration();
    double viewportArgumentsLength(double length) const;
    double initialScaleFromSize(double width, double height, bool shouldIgnoreScalingConstraints) const;
    int layoutWidth() const;
    int layoutHeight() const;

    bool shouldIgnoreScalingConstraints() const;
    bool shouldIgnoreVerticalScalingConstraints() const;
    bool shouldIgnoreHorizontalScalingConstraints() const;

    Parameters m_configuration;
    Parameters m_defaultConfiguration;
    IntSize m_contentSize;
    FloatSize m_minimumLayoutSize;
    ViewportArguments m_viewportArguments;

    bool m_canIgnoreScalingConstraints;
    bool m_forceAlwaysUserScalable;
};

TextStream& operator<<(TextStream&, const ViewportConfiguration::Parameters&);

} // namespace WebCore

#endif // ViewportConfiguration_h
