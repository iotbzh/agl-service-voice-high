/*
 * Copyright 2018-2019 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *     http://aws.amazon.com/apache2.0/
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <gtest/gtest.h>

#include "core/VRRequestProcessor.h"
#include "voiceagents/include/VoiceAgent.h"
#include "voiceagents/test/VoiceAgentsTestData.h"

#include "test/common/ConsoleLogger.h"
#include "test/mocks/AFBApiMock.h"

using namespace vshl::core;
using namespace vshl::voiceagents;
using namespace vshl::test::common;

namespace vshl {
namespace test {

class VRRequestProcessorTest : public ::testing::Test {
protected:
    void SetUp() override {
        mConsoleLogger = std::make_shared<ConsoleLogger>();
        mAfbApi = std::make_shared<::testing::StrictMock<AFBApiMock>>();

        auto vaTestData = *(getVoiceAgentsTestData().begin());
        mVoiceAgent = VoiceAgent::create(
            mConsoleLogger,
            vaTestData.id,
            vaTestData.name,
            vaTestData.description,
            vaTestData.api,
            vaTestData.vendor,
            vaTestData.activeWakeword,
            vaTestData.isActive,
            vaTestData.wakewords);

        mVRReqProcessorDelegate = VRRequestProcessorDelegate::create(mConsoleLogger, mAfbApi);
        mVRRequestProcessor = VRRequestProcessor::create(mConsoleLogger, mVRReqProcessorDelegate);
    }

    std::shared_ptr<::testing::StrictMock<AFBApiMock>> mAfbApi;
    std::shared_ptr<ConsoleLogger> mConsoleLogger;
    std::shared_ptr<VoiceAgent> mVoiceAgent;

    std::shared_ptr<VRRequestProcessorDelegate> mVRReqProcessorDelegate;
    std::shared_ptr<VRRequestProcessor> mVRRequestProcessor;
};

TEST_F(VRRequestProcessorTest, initializesCorrectly) {
    ASSERT_NE(mVRRequestProcessor, nullptr);
}

TEST_F(VRRequestProcessorTest, startListeningFailsOnLackOfDefaultAgent) {
    auto requestId = mVRRequestProcessor->startListening();
    ASSERT_EQ(requestId, "");
}

TEST_F(VRRequestProcessorTest, startListeningAndCancelWorks) {
    mVRReqProcessorDelegate->setDefaultVoiceAgent(mVoiceAgent);

    {
        ::testing::InSequence dummy;

        EXPECT_CALL(
            *mAfbApi,
            callSync(
                mVoiceAgent->getApi(),
                VRRequest::VA_VERB_STARTLISTENING,
                ::testing::_,
                ::testing::_,
                ::testing::_,
                ::testing::_))
            .Times(1);

        EXPECT_CALL(
            *mAfbApi,
            callSync(
                mVoiceAgent->getApi(),
                VRRequest::VA_VERB_CANCEL,
                ::testing::_,
                ::testing::_,
                ::testing::_,
                ::testing::_))
            .Times(1);
    }

    mVRRequestProcessor->startListening();
    auto requests = mVRReqProcessorDelegate->getAllRequests();
    ASSERT_EQ(requests.size(), 1);

    mVRRequestProcessor->cancel();
    requests = mVRReqProcessorDelegate->getAllRequests();
    ASSERT_EQ(requests.size(), 0);
}

TEST_F(VRRequestProcessorTest, requestIsCancelledOnObjectDestruction) {
    auto delegate = VRRequestProcessorDelegate::create(mConsoleLogger, mAfbApi);
    auto processor = VRRequestProcessor::create(mConsoleLogger, delegate);

    delegate->setDefaultVoiceAgent(mVoiceAgent);

    {
        ::testing::InSequence dummy;

        EXPECT_CALL(
            *mAfbApi,
            callSync(
                mVoiceAgent->getApi(),
                VRRequest::VA_VERB_STARTLISTENING,
                ::testing::_,
                ::testing::_,
                ::testing::_,
                ::testing::_))
            .Times(1);

        EXPECT_CALL(
            *mAfbApi,
            callSync(
                mVoiceAgent->getApi(),
                VRRequest::VA_VERB_CANCEL,
                ::testing::_,
                ::testing::_,
                ::testing::_,
                ::testing::_))
            .Times(1);
    }

    auto requestId = processor->startListening();
    ASSERT_NE(requestId, "");

    auto requests = delegate->getAllRequests();
    ASSERT_EQ(requests.size(), 1);
}

TEST_F(VRRequestProcessorTest, backToBackStartListeningCancelsEarlierRequest) {
    mVRReqProcessorDelegate->setDefaultVoiceAgent(mVoiceAgent);

    {
        ::testing::InSequence dummy;

        EXPECT_CALL(
            *mAfbApi,
            callSync(
                mVoiceAgent->getApi(),
                VRRequest::VA_VERB_STARTLISTENING,
                ::testing::_,
                ::testing::_,
                ::testing::_,
                ::testing::_))
            .Times(1);

        EXPECT_CALL(
            *mAfbApi,
            callSync(
                mVoiceAgent->getApi(),
                VRRequest::VA_VERB_CANCEL,
                ::testing::_,
                ::testing::_,
                ::testing::_,
                ::testing::_))
            .Times(1);

        EXPECT_CALL(
            *mAfbApi,
            callSync(
                mVoiceAgent->getApi(),
                VRRequest::VA_VERB_STARTLISTENING,
                ::testing::_,
                ::testing::_,
                ::testing::_,
                ::testing::_))
            .Times(1);

        EXPECT_CALL(
            *mAfbApi,
            callSync(
                mVoiceAgent->getApi(),
                VRRequest::VA_VERB_CANCEL,
                ::testing::_,
                ::testing::_,
                ::testing::_,
                ::testing::_))
            .Times(1);
    }

    mVRRequestProcessor->startListening();
    auto requests = mVRReqProcessorDelegate->getAllRequests();
    ASSERT_EQ(requests.size(), 1);

    mVRRequestProcessor->startListening();
    requests = mVRReqProcessorDelegate->getAllRequests();
    ASSERT_EQ(requests.size(), 1);

    mVRRequestProcessor->cancel();
    requests = mVRReqProcessorDelegate->getAllRequests();
    ASSERT_EQ(requests.size(), 0);
}

}  // namespace test
}  // namespace vshl