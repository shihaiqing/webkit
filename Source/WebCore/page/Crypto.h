/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 * Copyright (C) 2013 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer. 
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution. 
 * 3.  Neither the name of Google, Inc. ("Google") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY GOOGLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef Crypto_h
#define Crypto_h

#include "ContextDestructionObserver.h"
#include <wtf/Forward.h>
#include <wtf/RefCounted.h>
#include <wtf/RefPtr.h>

namespace JSC {
class ArrayBufferView;
}

namespace WebCore {

typedef int ExceptionCode;

class Document;
class WebKitSubtleCrypto;
class SubtleCrypto;

class Crypto : public ContextDestructionObserver, public RefCounted<Crypto> {
public:
    static Ref<Crypto> create(ScriptExecutionContext& context) { return adoptRef(*new Crypto(context)); }
    virtual ~Crypto();

    void getRandomValues(JSC::ArrayBufferView*, ExceptionCode&);

#if ENABLE(SUBTLE_CRYPTO)
    SubtleCrypto& subtle();

    // Will be deprecated.
    WebKitSubtleCrypto* webkitSubtle(ExceptionCode&);
#endif

private:
    Crypto(ScriptExecutionContext&);

#if ENABLE(SUBTLE_CRYPTO)
    Ref<SubtleCrypto> m_subtle;

    // Will be deprecated.
    RefPtr<WebKitSubtleCrypto> m_webkitSubtle;
#endif
};

}

#endif
