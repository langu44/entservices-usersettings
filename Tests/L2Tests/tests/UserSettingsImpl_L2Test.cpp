/**
* If not stated otherwise in this file or this component's LICENSE
* file the following copyright and licenses apply:
*
* Copyright 2024 RDK Management
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
**/

#include <gtest/gtest.h>

// Including UserSettingsImpl.cpp directly is intentional in this translation unit.
// plugin/IUserSettings.h defines Exchange::IUserSettings with a different method set
// than the system-wide <interfaces/IUserSettings.h>, causing an ODR violation when
// both headers are included in the same binary. By placing this test in a separate
// build target (see CMakeLists.txt) with ../../plugin on the include path, we ensure
// that #include "IUserSettings.h" inside UserSettingsImpl.cpp resolves to the local
// header rather than the system one. This avoids the ODR conflict and allows direct
// unit testing of UserSettingsImpl without the Thunder COM-RPC infrastructure.
#include "../../plugin/UserSettingsImpl.cpp" // NOLINT(build/include)

namespace {

// ConcreteUserSettingsImpl provides minimal IUnknown stubs required to
// instantiate the otherwise-abstract UserSettingsImpl in unit tests.
// Reference counting is intentionally disabled (returning 1) because these
// objects have stack/heap lifetimes managed directly by the test fixture and
// do not participate in COM-RPC reference counting.
class ConcreteUserSettingsImpl : public WPEFramework::Plugin::UserSettingsImpl {
public:
    uint32_t AddRef() const override { return 1; } // stub: test manages lifetime directly
    uint32_t Release() const override { return 1; } // stub: test manages lifetime directly
    void* QueryInterface(const uint32_t /*id*/) override { return nullptr; }
};

class TestNotificationHandler : public WPEFramework::Exchange::IUserSettings::INotification {
public:
    int audioChangedCount = 0;
    bool lastAudioEnabled = false;
    int resolutionChangedCount = 0;
    WPEFramework::Exchange::IUserSettings::Resolution lastResolution{};

    void OnAudioOutputChanged(bool enabled) override {
        audioChangedCount++;
        lastAudioEnabled = enabled;
    }

    void OnResolutionChanged(WPEFramework::Exchange::IUserSettings::Resolution resolution) override {
        resolutionChangedCount++;
        lastResolution = resolution;
    }

    // IUnknown stubs: test manages lifetime directly, no COM-RPC refcounting needed.
    uint32_t AddRef() const override { return 1; }
    uint32_t Release() const override { return 1; }
    void* QueryInterface(const uint32_t /*id*/) override { return nullptr; }
};

class UserSettingsImplTest : public ::testing::Test {
protected:
    ConcreteUserSettingsImpl* m_impl{nullptr};

    void SetUp() override {
        m_impl = new ConcreteUserSettingsImpl();
    }

    void TearDown() override {
        delete m_impl;
        m_impl = nullptr;
    }
};

// ---------------------------------------------------------------------------
// Audio tests
// ---------------------------------------------------------------------------

TEST_F(UserSettingsImplTest, GetAudioOutputEnabled_DefaultIsEnabled)
{
    bool enabled = false;
    EXPECT_EQ(0u, m_impl->GetAudioOutputEnabled(enabled));
    EXPECT_TRUE(enabled);
}

TEST_F(UserSettingsImplTest, SetAudioOutputEnabled_DisablesAudio)
{
    EXPECT_EQ(0u, m_impl->SetAudioOutputEnabled(false));
    bool enabled = true;
    EXPECT_EQ(0u, m_impl->GetAudioOutputEnabled(enabled));
    EXPECT_FALSE(enabled);
}

TEST_F(UserSettingsImplTest, SetAudioOutputEnabled_EnablesAudio)
{
    EXPECT_EQ(0u, m_impl->SetAudioOutputEnabled(false));
    EXPECT_EQ(0u, m_impl->SetAudioOutputEnabled(true));
    bool enabled = false;
    EXPECT_EQ(0u, m_impl->GetAudioOutputEnabled(enabled));
    EXPECT_TRUE(enabled);
}

TEST_F(UserSettingsImplTest, SetAudioOutputEnabled_SameValueDoesNotFireNotification)
{
    TestNotificationHandler handler;
    m_impl->Register(&handler);

    // Default is true; setting true again must NOT fire a notification
    EXPECT_EQ(0u, m_impl->SetAudioOutputEnabled(true));
    EXPECT_EQ(0, handler.audioChangedCount);

    m_impl->Unregister(&handler);
}

TEST_F(UserSettingsImplTest, SetAudioOutputEnabled_DifferentValueFiresNotification)
{
    TestNotificationHandler handler;
    m_impl->Register(&handler);

    EXPECT_EQ(0u, m_impl->SetAudioOutputEnabled(false));
    EXPECT_EQ(1, handler.audioChangedCount);
    EXPECT_FALSE(handler.lastAudioEnabled);

    m_impl->Unregister(&handler);
}

// ---------------------------------------------------------------------------
// Resolution tests
// ---------------------------------------------------------------------------

TEST_F(UserSettingsImplTest, GetDisplayResolution_DefaultIs1080P)
{
    WPEFramework::Exchange::IUserSettings::Resolution resolution{};
    EXPECT_EQ(0u, m_impl->GetDisplayResolution(resolution));
    EXPECT_EQ(WPEFramework::Exchange::IUserSettings::Resolution::RESOLUTION_1080P, resolution);
}

TEST_F(UserSettingsImplTest, SetDisplayResolution_ChangesTo720P)
{
    EXPECT_EQ(0u, m_impl->SetDisplayResolution(
        WPEFramework::Exchange::IUserSettings::Resolution::RESOLUTION_720P));
    WPEFramework::Exchange::IUserSettings::Resolution resolution{};
    EXPECT_EQ(0u, m_impl->GetDisplayResolution(resolution));
    EXPECT_EQ(WPEFramework::Exchange::IUserSettings::Resolution::RESOLUTION_720P, resolution);
}

TEST_F(UserSettingsImplTest, SetDisplayResolution_ChangesTo4K)
{
    EXPECT_EQ(0u, m_impl->SetDisplayResolution(
        WPEFramework::Exchange::IUserSettings::Resolution::RESOLUTION_4K));
    WPEFramework::Exchange::IUserSettings::Resolution resolution{};
    EXPECT_EQ(0u, m_impl->GetDisplayResolution(resolution));
    EXPECT_EQ(WPEFramework::Exchange::IUserSettings::Resolution::RESOLUTION_4K, resolution);
}

TEST_F(UserSettingsImplTest, SetDisplayResolution_AlwaysFiresNotification)
{
    TestNotificationHandler handler;
    m_impl->Register(&handler);

    // Resolution always fires (even when value is the same)
    EXPECT_EQ(0u, m_impl->SetDisplayResolution(
        WPEFramework::Exchange::IUserSettings::Resolution::RESOLUTION_720P));
    EXPECT_EQ(1, handler.resolutionChangedCount);
    EXPECT_EQ(WPEFramework::Exchange::IUserSettings::Resolution::RESOLUTION_720P,
              handler.lastResolution);

    m_impl->Unregister(&handler);
}

// ---------------------------------------------------------------------------
// Registration / notification-handler tests
// ---------------------------------------------------------------------------

TEST_F(UserSettingsImplTest, Register_NullPointerDoesNotCrash)
{
    EXPECT_NO_FATAL_FAILURE(m_impl->Register(nullptr));
}

TEST_F(UserSettingsImplTest, Unregister_NotRegisteredDoesNotCrash)
{
    TestNotificationHandler handler;
    EXPECT_NO_FATAL_FAILURE(m_impl->Unregister(&handler));
}

TEST_F(UserSettingsImplTest, MultipleNotificationHandlers_AllReceiveNotification)
{
    TestNotificationHandler handler1;
    TestNotificationHandler handler2;
    m_impl->Register(&handler1);
    m_impl->Register(&handler2);

    EXPECT_EQ(0u, m_impl->SetAudioOutputEnabled(false));
    EXPECT_EQ(1, handler1.audioChangedCount);
    EXPECT_EQ(1, handler2.audioChangedCount);

    m_impl->Unregister(&handler1);
    m_impl->Unregister(&handler2);
}

TEST_F(UserSettingsImplTest, UnregisterHandler_StopsReceivingNotifications)
{
    TestNotificationHandler handler;
    m_impl->Register(&handler);
    m_impl->Unregister(&handler);

    EXPECT_EQ(0u, m_impl->SetAudioOutputEnabled(false));
    EXPECT_EQ(0, handler.audioChangedCount);
}

TEST_F(UserSettingsImplTest, Register_ValidHandler_ReceivesAudioNotification)
{
    TestNotificationHandler handler;
    m_impl->Register(&handler);

    EXPECT_EQ(0u, m_impl->SetAudioOutputEnabled(false));
    EXPECT_EQ(1, handler.audioChangedCount);
    EXPECT_FALSE(handler.lastAudioEnabled);

    m_impl->Unregister(&handler);
}

TEST_F(UserSettingsImplTest, Register_ValidHandler_ReceivesResolutionNotification)
{
    TestNotificationHandler handler;
    m_impl->Register(&handler);

    EXPECT_EQ(0u, m_impl->SetDisplayResolution(
        WPEFramework::Exchange::IUserSettings::Resolution::RESOLUTION_4K));
    EXPECT_EQ(1, handler.resolutionChangedCount);
    EXPECT_EQ(WPEFramework::Exchange::IUserSettings::Resolution::RESOLUTION_4K,
              handler.lastResolution);

    m_impl->Unregister(&handler);
}

} // anonymous namespace
