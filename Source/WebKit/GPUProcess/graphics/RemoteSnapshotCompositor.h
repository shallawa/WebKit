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

#pragma once

#include <wtf/HashMap.h>

namespace WebCore {
class SharedBuffer;
}

namespace WebKit {

class RemoteSnapshotCompositor {
public:
    static std::unique_ptr<RemoteSnapshotCompositor> create(WebCore::FrameIdentifier, Ref<WebCore::ImageBuffer>&&);

    void addFrameResource(WebCore::FrameIdentifier, WebCore::FrameIdentifier parentFrameIdentifier, Ref<WebCore::ImageBuffer>&&);

    RefPtr<WebCore::SharedBuffer> sinkToPDFDocument();

private:
    RemoteSnapshotCompositor(WebCore::FrameIdentifier, Ref<WebCore::ImageBuffer>&&);

    WebCore::ImageBuffer* frameImageBuffer(WebCore::FrameIdentifier);

    WebCore::FrameIdentifier m_frameIdentifier;
    HashMap<WebCore::FrameIdentifier, Ref<WebCore::ImageBuffer>> m_frameImageBufferMap;
};

} // namespace WebKit
