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
#include "ImageBufferCGPDFDocumentBackend.h"

#if USE(CG)

#include "GraphicsContext.h"
#include "GraphicsContextCG.h"
#include <wtf/TZoneMallocInlines.h>

WTF_ALLOW_UNSAFE_BUFFER_USAGE_BEGIN

namespace WebCore {

WTF_MAKE_TZONE_OR_ISO_ALLOCATED_IMPL(ImageBufferCGPDFDocumentBackend);

size_t ImageBufferCGPDFDocumentBackend::calculateMemoryCost(const Parameters& parameters)
{
    return ImageBufferBackend::calculateMemoryCost(parameters.backendSize, calculateBytesPerRow(parameters.backendSize));
}

std::unique_ptr<ImageBufferCGPDFDocumentBackend> ImageBufferCGPDFDocumentBackend::create(const Parameters& parameters, const ImageBufferCreationContext&)
{
    auto data = adoptCF(CFDataCreateMutable(kCFAllocatorDefault, 0));

    auto dataConsumer = adoptCF(CGDataConsumerCreateWithCFData(data.get()));

    auto backendSize = parameters.backendSize;
    auto mediaBox = CGRectMake(0, 0, backendSize.width(), backendSize.height());

    auto pdfContext = adoptCF(CGPDFContextCreate(dataConsumer.get(), &mediaBox, nullptr));
    auto context = makeUnique<GraphicsContextCG>(pdfContext.get());

    return std::unique_ptr<ImageBufferCGPDFDocumentBackend>(new ImageBufferCGPDFDocumentBackend(parameters, WTFMove(context), WTFMove(data)));
}

ImageBufferCGPDFDocumentBackend::ImageBufferCGPDFDocumentBackend(const Parameters& parameters, std::unique_ptr<GraphicsContextCG>&& context, RetainPtr<CFDataRef>&& data)
    : ImageBufferCGBackend(parameters, WTFMove(context))
    , m_data(WTFMove(data))
{
}

ImageBufferCGPDFDocumentBackend::~ImageBufferCGPDFDocumentBackend() = default;

RefPtr<SharedBuffer> ImageBufferCGPDFDocumentBackend::sinkToPDFDocument()
{
    CGPDFContextClose(m_context->platformContext());
    return SharedBuffer::create(m_data.get());
}

String ImageBufferCGPDFDocumentBackend::debugDescription() const
{
    TextStream stream;
    stream << "ImageBufferCGPDFDocumentBackend " << this;
    return stream.release();
}

} // namespace WebCore

WTF_ALLOW_UNSAFE_BUFFER_USAGE_END

#endif // USE(CG)
