/*
 * Copyright (C) 2024 Apple Inc.  All rights reserved.
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
#include "RemoteSnapshotCompositor.h"

#include <WebCore/DisplayListRecorderImpl.h>
#include <WebCore/GraphicsContext.h>
#include <WebCore/ImageBuffer.h>
#include <WebCore/SharedBuffer.h>

namespace WebKit {
using namespace WebCore;

std::unique_ptr<RemoteSnapshotCompositor> RemoteSnapshotCompositor::create(FrameIdentifier frameIdentifier, Ref<ImageBuffer>&& buffer)
{
    return std::unique_ptr<RemoteSnapshotCompositor>(new RemoteSnapshotCompositor(frameIdentifier, WTFMove(buffer)));
}

RemoteSnapshotCompositor::RemoteSnapshotCompositor(FrameIdentifier frameIdentifier, Ref<ImageBuffer>&& buffer)
    : m_frameIdentifier(frameIdentifier)
{
    m_frameImageBufferMap.add(frameIdentifier, WTFMove(buffer));
}

void RemoteSnapshotCompositor::addFrameResource(FrameIdentifier frameIdentifier, FrameIdentifier parentFrameIdentifier, Ref<ImageBuffer>&& buffer)
{
    ASSERT(!frameImageBuffer(frameIdentifier));
    m_frameImageBufferMap.add(frameIdentifier, Ref { buffer });

    RefPtr parentFrameBuffer = frameImageBuffer(parentFrameIdentifier);
    if (!parentFrameBuffer)
        return;

    auto& context = parentFrameBuffer->context();

    auto* recorder = dynamicDowncast<DisplayList::RecorderImpl>(context);
    if (!recorder)
        return;

    recorder->recordframeImageBufferUse(frameIdentifier, WTFMove(buffer));
}

ImageBuffer* RemoteSnapshotCompositor::frameImageBuffer(FrameIdentifier frameIdentifier)
{
    return m_frameImageBufferMap.get(frameIdentifier);
}

RefPtr<SharedBuffer> RemoteSnapshotCompositor::sinkToPDFDocument()
{
    RefPtr mainFrameBuffer = frameImageBuffer(m_frameIdentifier);
    if (!mainFrameBuffer)
        return nullptr;

    auto size = mainFrameBuffer->logicalSize();
    auto resolutionScale = mainFrameBuffer->resolutionScale();

    RefPtr buffer = ImageBuffer::create(size, RenderingMode::PDFDocument, RenderingPurpose::Snapshot, resolutionScale, DestinationColorSpace::SRGB(), ImageBufferPixelFormat::BGRA8);
    if (!buffer)
        return nullptr;

    auto& context = buffer->context();
    context.drawImageBuffer(*mainFrameBuffer, FloatPoint::zero());

    return buffer->sinkToPDFDocument();
}

} // namespace WebKit
