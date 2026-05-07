#include <gtest/gtest.h>

#include "UserSettingsImpl.h"

using namespace WPEFramework;

TEST(UserSettingsImplL2Test, GetAudioOutputEnabled_DefaultValue)
{
    Plugin::UserSettingsImpl impl;
    bool enabled = false;

    EXPECT_EQ(Core::ERROR_NONE, impl.GetAudioOutputEnabled(enabled));
    EXPECT_TRUE(enabled);
}

TEST(UserSettingsImplL2Test, SetAudioOutputEnabled_UpdatesState)
{
    Plugin::UserSettingsImpl impl;
    bool enabled = true;

    EXPECT_EQ(Core::ERROR_NONE, impl.SetAudioOutputEnabled(false));
    EXPECT_EQ(Core::ERROR_NONE, impl.GetAudioOutputEnabled(enabled));
    EXPECT_FALSE(enabled);
}

TEST(UserSettingsImplL2Test, SetDisplayResolution_UpdatesState)
{
    Plugin::UserSettingsImpl impl;
    Exchange::IUserSettings::Resolution resolution = Exchange::IUserSettings::Resolution::RESOLUTION_720P;

    EXPECT_EQ(Core::ERROR_NONE, impl.SetDisplayResolution(Exchange::IUserSettings::Resolution::RESOLUTION_4K));
    EXPECT_EQ(Core::ERROR_NONE, impl.GetDisplayResolution(resolution));
    EXPECT_EQ(Exchange::IUserSettings::Resolution::RESOLUTION_4K, resolution);
}
