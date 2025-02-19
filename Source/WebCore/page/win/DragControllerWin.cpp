/*
 * Copyright (C) 2007 Apple Inc.  All rights reserved.
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
#include "DragController.h"

#include "DataTransfer.h"
#include "Document.h"
#include "DragData.h"
#include "Element.h"
#include "Pasteboard.h"
#include "markup.h"
#include "windows.h"

namespace WebCore {

const int DragController::LinkDragBorderInset = 2;
const int DragController::MaxOriginalImageArea = 1500 * 1500;
const int DragController::DragIconRightInset = 7;
const int DragController::DragIconBottomInset = 3;

const float DragController::DragImageAlpha = 0.75f;

DragOperation DragController::dragOperation(DragData& dragData)
{
    //FIXME: to match the macos behaviour we should return DragOperationNone
    //if we are a modal window, we are the drag source, or the window is an attached sheet
    //If this can be determined from within WebCore operationForDrag can be pulled into 
    //WebCore itself
    return dragData.containsURL() && !m_didInitiateDrag ? DragOperationCopy : DragOperationNone;
}

bool DragController::isCopyKeyDown(DragData&)
{
    return ::GetAsyncKeyState(VK_CONTROL);
}
    
const IntSize& DragController::maxDragImageSize()
{
    static const IntSize maxDragImageSize(200, 200);
    
    return maxDragImageSize;
}

void DragController::cleanupAfterSystemDrag()
{
}

#if ENABLE(ATTACHMENT_ELEMENT)
void DragController::declareAndWriteAttachment(DataTransfer&, Element&, const URL&)
{
}
#endif

void DragController::declareAndWriteDragImage(DataTransfer& dataTransfer, Element& element, const URL& url, const String& label)
{
    Pasteboard& pasteboard = dataTransfer.pasteboard();

    // FIXME: Do we really need this check?
    if (!pasteboard.writableDataObject())
        return;

    // Order is important here for Explorer's sake
    pasteboard.writeURLToWritableDataObject(url, label);
    pasteboard.writeImageToDataObject(element, url);
    pasteboard.writeMarkup(createMarkup(element, IncludeNode, 0, ResolveAllURLs));
}

}
