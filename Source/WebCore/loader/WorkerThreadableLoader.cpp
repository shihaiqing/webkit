/*
 * Copyright (C) 2009, 2010 Google Inc. All rights reserved.
 * Copyright (C) 2016 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "WorkerThreadableLoader.h"

#include "ContentSecurityPolicy.h"
#include "Document.h"
#include "DocumentThreadableLoader.h"
#include "ResourceError.h"
#include "ResourceRequest.h"
#include "ResourceResponse.h"
#include "SecurityOrigin.h"
#include "ThreadableLoader.h"
#include "WorkerGlobalScope.h"
#include "WorkerLoaderProxy.h"
#include "WorkerThread.h"
#include <wtf/MainThread.h>
#include <wtf/Vector.h>

namespace WebCore {

static const char loadResourceSynchronouslyMode[] = "loadResourceSynchronouslyMode";

WorkerThreadableLoader::WorkerThreadableLoader(WorkerGlobalScope& workerGlobalScope, ThreadableLoaderClient& client, const String& taskMode, ResourceRequest&& request, const ThreadableLoaderOptions& options, const String& referrer)
    : m_workerGlobalScope(workerGlobalScope)
    , m_workerClientWrapper(ThreadableLoaderClientWrapper::create(client))
    , m_bridge(*new MainThreadBridge(m_workerClientWrapper.get(), workerGlobalScope.thread().workerLoaderProxy(), taskMode, WTFMove(request), options, referrer.isEmpty() ? workerGlobalScope.url().strippedForUseAsReferrer() : referrer, workerGlobalScope.securityOrigin(), workerGlobalScope.contentSecurityPolicy()))
{
}

WorkerThreadableLoader::~WorkerThreadableLoader()
{
    m_bridge.destroy();
}

void WorkerThreadableLoader::loadResourceSynchronously(WorkerGlobalScope& workerGlobalScope, ResourceRequest&& request, ThreadableLoaderClient& client, const ThreadableLoaderOptions& options)
{
    WorkerRunLoop& runLoop = workerGlobalScope.thread().runLoop();

    // Create a unique mode just for this synchronous resource load.
    String mode = loadResourceSynchronouslyMode;
    mode.append(String::number(runLoop.createUniqueId()));

    RefPtr<WorkerThreadableLoader> loader = WorkerThreadableLoader::create(workerGlobalScope, client, mode, WTFMove(request), options, String());
    MessageQueueWaitResult result = MessageQueueMessageReceived;
    while (!loader->done() && result != MessageQueueTerminated)
        result = runLoop.runInMode(&workerGlobalScope, mode);

    if (!loader->done() && result == MessageQueueTerminated)
        loader->cancel();
}

void WorkerThreadableLoader::cancel()
{
    m_bridge.cancel();
}

struct LoaderTaskOptions {
    LoaderTaskOptions(const ThreadableLoaderOptions&, const String&, Ref<SecurityOrigin>&&);
    ThreadableLoaderOptions options;
    String referrer;
    Ref<SecurityOrigin> origin;
};

LoaderTaskOptions::LoaderTaskOptions(const ThreadableLoaderOptions& options, const String& referrer, Ref<SecurityOrigin>&& origin)
    : options(options, options.preflightPolicy, options.contentSecurityPolicyEnforcement, options.initiator.isolatedCopy(), options.opaqueResponse)
    , referrer(referrer.isolatedCopy())
    , origin(WTFMove(origin))
{
}

WorkerThreadableLoader::MainThreadBridge::MainThreadBridge(ThreadableLoaderClientWrapper& workerClientWrapper, WorkerLoaderProxy& loaderProxy, const String& taskMode,
    ResourceRequest&& request, const ThreadableLoaderOptions& options, const String& outgoingReferrer,
    const SecurityOrigin* securityOrigin, const ContentSecurityPolicy* contentSecurityPolicy)
    : m_workerClientWrapper(&workerClientWrapper)
    , m_loaderProxy(loaderProxy)
    , m_taskMode(taskMode.isolatedCopy())
{
    ASSERT(securityOrigin);
    ASSERT(contentSecurityPolicy);

    auto securityOriginCopy = securityOrigin->isolatedCopy();
    auto contentSecurityPolicyCopy = std::make_unique<ContentSecurityPolicy>(securityOriginCopy);
    contentSecurityPolicyCopy->copyStateFrom(contentSecurityPolicy);

    auto optionsCopy = std::make_unique<LoaderTaskOptions>(options, request.httpReferrer().isNull() ? outgoingReferrer : request.httpReferrer(), WTFMove(securityOriginCopy));

    // Can we benefit from request being an r-value to create more efficiently its isolated copy?
    m_loaderProxy.postTaskToLoader([this, request = request.isolatedCopy(), options = WTFMove(optionsCopy), contentSecurityPolicyCopy = WTFMove(contentSecurityPolicyCopy)](ScriptExecutionContext& context) mutable {
        ASSERT(isMainThread());
        Document& document = downcast<Document>(context);

        // FIXME: If the site requests a local resource, then this will return a non-zero value but the sync path will return a 0 value.
        // Either this should return 0 or the other code path should call a failure callback.
        m_mainThreadLoader = DocumentThreadableLoader::create(document, *this, WTFMove(request), options->options, WTFMove(options->origin), WTFMove(contentSecurityPolicyCopy), WTFMove(options->referrer));
        ASSERT(m_mainThreadLoader || m_loadingFinished);
    });
}

void WorkerThreadableLoader::MainThreadBridge::destroy()
{
    // Ensure that no more client callbacks are done in the worker context's thread.
    clearClientWrapper();

    // "delete this" and m_mainThreadLoader::deref() on the worker object's thread.
    m_loaderProxy.postTaskToLoader([self = std::unique_ptr<WorkerThreadableLoader::MainThreadBridge>(this)] (ScriptExecutionContext& context) {
        ASSERT(isMainThread());
        ASSERT_UNUSED(context, context.isDocument());
    });
}

void WorkerThreadableLoader::MainThreadBridge::cancel()
{
    m_loaderProxy.postTaskToLoader([this] (ScriptExecutionContext& context) {
        ASSERT(isMainThread());
        ASSERT_UNUSED(context, context.isDocument());

        if (!m_mainThreadLoader)
            return;
        m_mainThreadLoader->cancel();
        m_mainThreadLoader = nullptr;
    });

    if (m_workerClientWrapper->done()) {
        clearClientWrapper();
        return;
    }
    // Taking a ref of client wrapper as call to didFail may take out the last reference of it.
    Ref<ThreadableLoaderClientWrapper> protectedWorkerClientWrapper(*m_workerClientWrapper);
    // If the client hasn't reached a termination state, then transition it by sending a cancellation error.
    // Note: no more client callbacks will be done after this method -- we clear the client wrapper to ensure that.
    ResourceError error(ResourceError::Type::Cancellation);
    protectedWorkerClientWrapper->didFail(error);
    protectedWorkerClientWrapper->clearClient();
}

void WorkerThreadableLoader::MainThreadBridge::clearClientWrapper()
{
    m_workerClientWrapper->clearClient();
}

void WorkerThreadableLoader::MainThreadBridge::didSendData(unsigned long long bytesSent, unsigned long long totalBytesToBeSent)
{
    Ref<ThreadableLoaderClientWrapper> protectedWorkerClientWrapper = *m_workerClientWrapper;
    m_loaderProxy.postTaskForModeToWorkerGlobalScope([protectedWorkerClientWrapper = WTFMove(protectedWorkerClientWrapper), bytesSent, totalBytesToBeSent] (ScriptExecutionContext& context) mutable {
        ASSERT_UNUSED(context, context.isWorkerGlobalScope());
        protectedWorkerClientWrapper->didSendData(bytesSent, totalBytesToBeSent);
    }, m_taskMode);
}

void WorkerThreadableLoader::MainThreadBridge::didReceiveResponse(unsigned long identifier, const ResourceResponse& response)
{
    Ref<ThreadableLoaderClientWrapper> protectedWorkerClientWrapper = *m_workerClientWrapper;
    m_loaderProxy.postTaskForModeToWorkerGlobalScope([protectedWorkerClientWrapper = WTFMove(protectedWorkerClientWrapper), identifier, responseData = response.crossThreadData()] (ScriptExecutionContext& context) mutable {
        ASSERT_UNUSED(context, context.isWorkerGlobalScope());
        protectedWorkerClientWrapper->didReceiveResponse(identifier, ResourceResponse::fromCrossThreadData(WTFMove(responseData)));
    }, m_taskMode);
}

void WorkerThreadableLoader::MainThreadBridge::didReceiveData(const char* data, int dataLength)
{
    Ref<ThreadableLoaderClientWrapper> protectedWorkerClientWrapper = *m_workerClientWrapper;
    Vector<char> vector(dataLength);
    memcpy(vector.data(), data, dataLength);
    m_loaderProxy.postTaskForModeToWorkerGlobalScope([protectedWorkerClientWrapper = WTFMove(protectedWorkerClientWrapper), vector = WTFMove(vector)] (ScriptExecutionContext& context) mutable {
        ASSERT_UNUSED(context, context.isWorkerGlobalScope());
        protectedWorkerClientWrapper->didReceiveData(vector.data(), vector.size());
    }, m_taskMode);
}

void WorkerThreadableLoader::MainThreadBridge::didFinishLoading(unsigned long identifier, double finishTime)
{
    m_loadingFinished = true;
    Ref<ThreadableLoaderClientWrapper> protectedWorkerClientWrapper = *m_workerClientWrapper;
    m_loaderProxy.postTaskForModeToWorkerGlobalScope([protectedWorkerClientWrapper = WTFMove(protectedWorkerClientWrapper), identifier, finishTime] (ScriptExecutionContext& context) mutable {
        ASSERT_UNUSED(context, context.isWorkerGlobalScope());
        protectedWorkerClientWrapper->didFinishLoading(identifier, finishTime);
    }, m_taskMode);
}

void WorkerThreadableLoader::MainThreadBridge::didFail(const ResourceError& error)
{
    m_loadingFinished = true;
    m_loaderProxy.postTaskForModeToWorkerGlobalScope([workerClientWrapper = Ref<ThreadableLoaderClientWrapper>(*m_workerClientWrapper), error = error.isolatedCopy()] (ScriptExecutionContext& context) mutable {
        ASSERT_UNUSED(context, context.isWorkerGlobalScope());
        workerClientWrapper->didFail(error);
    }, m_taskMode);
}

} // namespace WebCore
