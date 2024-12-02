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
#include "ImageBufferRemoteDisplayListBackend.h"

#include <wtf/TZoneMallocInlines.h>

namespace WebKit {
using namespace WebCore;

WTF_MAKE_TZONE_OR_ISO_ALLOCATED_IMPL(ImageBufferRemoteDisplayListBackend);

std::unique_ptr<ImageBufferRemoteDisplayListBackend> ImageBufferRemoteDisplayListBackend::create(const Parameters& parameters, RemoteRenderingBackendProxy& remoteRenderingBackendProxy, RenderingResourceIdentifier imageBufferIdentifier)
{
    return std::unique_ptr<ImageBufferRemoteDisplayListBackend> { new ImageBufferRemoteDisplayListBackend { parameters, remoteRenderingBackendProxy, imageBufferIdentifier } };
}

unsigned ImageBufferRemoteDisplayListBackend::calculateBytesPerRow(const IntSize& backendSize)
{
    ASSERT(!backendSize.isEmpty());
    return CheckedUint32(backendSize.width()) * 4;
}

size_t ImageBufferRemoteDisplayListBackend::calculateMemoryCost(const Parameters& parameters)
{
    return ImageBufferBackend::calculateMemoryCost(parameters.backendSize, calculateBytesPerRow(parameters.backendSize));
}

ImageBufferRemoteDisplayListBackend::ImageBufferRemoteDisplayListBackend(const Parameters& parameters, RemoteRenderingBackendProxy& remoteRenderingBackendProxy, RenderingResourceIdentifier imageBufferIdentifier)
    : WebCore::NullImageBufferBackend(parameters)
    , m_remoteRenderingBackendProxy(remoteRenderingBackendProxy)
{
    ASSERT(m_parameters.snapshotParameters);
    auto& snapshotParameters = m_parameters.snapshotParameters;

    if (!snapshotParameters->snapshotIdentifier) {
        snapshotParameters->snapshotIdentifier = m_remoteRenderingBackendProxy->createSnapshotCompositor(snapshotParameters->frameIdentifier, imageBufferIdentifier);
        return;
    }

    ASSERT(snapshotParameters->parentFrameIdentifier);
    m_remoteRenderingBackendProxy->addSnapshotRemoteFrameResource(snapshotParameters->frameIdentifier, *snapshotParameters->snapshotIdentifier, *snapshotParameters->parentFrameIdentifier, imageBufferIdentifier);
}

ImageBufferRemoteDisplayListBackend::~ImageBufferRemoteDisplayListBackend()
{
    ASSERT(m_parameters.snapshotParameters);
    auto& snapshotParameters = m_parameters.snapshotParameters;

    if (!snapshotParameters->snapshotIdentifier || snapshotParameters->parentFrameIdentifier || !m_remoteRenderingBackendProxy)
        return;

    m_remoteRenderingBackendProxy->releaseSnapshotCompositor(*snapshotParameters->snapshotIdentifier);
}

RefPtr<NativeImage> ImageBufferRemoteDisplayListBackend::createNativeImageReference()
{
    return nullptr;
}

String ImageBufferRemoteDisplayListBackend::debugDescription() const
{
    TextStream stream;
    stream << "ImageBufferRemoteDisplayListBackend " << this;
    return stream.release();
}

} // namespace WebCore
