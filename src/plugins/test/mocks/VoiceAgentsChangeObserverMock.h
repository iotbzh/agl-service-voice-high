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
#ifndef VSHL_TEST_MOCKS_VOICEAGENTSCHANGEOBSERVERMOCK_H_
#define VSHL_TEST_MOCKS_VOICEAGENTSCHANGEOBSERVERMOCK_H_

#include <gmock/gmock.h>

#include "interfaces/voiceagents/IVoiceAgentsChangeObserver.h"

namespace vshl {
namespace test {

class VoiceAgentsChangeObserverMock : public vshl::common::interfaces::IVoiceAgentsChangeObserver {
public:
    MOCK_METHOD1(OnDefaultVoiceAgentChanged, void(shared_ptr<vshl::common::interfaces::IVoiceAgent> defaultVoiceAgent));
    MOCK_METHOD1(OnVoiceAgentAdded, void(shared_ptr<vshl::common::interfaces::IVoiceAgent> voiceAgent));
    MOCK_METHOD1(OnVoiceAgentRemoved, void(shared_ptr<vshl::common::interfaces::IVoiceAgent> voiceAgent));
    MOCK_METHOD1(OnVoiceAgentActiveWakeWordChanged, void(shared_ptr<vshl::common::interfaces::IVoiceAgent> voiceAgent));
    MOCK_METHOD1(OnVoiceAgentActivated, void(shared_ptr<vshl::common::interfaces::IVoiceAgent> voiceAgent));
    MOCK_METHOD1(OnVoiceAgentDeactivated, void(shared_ptr<vshl::common::interfaces::IVoiceAgent> voiceAgent));
};

}  // namespace test
}  // namespace vshl

#endif  // VSHL_TEST_MOCKS_VOICEAGENTSCHANGEOBSERVERMOCK_H_