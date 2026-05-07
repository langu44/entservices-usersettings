/**
 * @file UserSettingsImpl.h
 * @brief User Settings plugin implementation declarations
 */

#pragma once

#include "IUserSettings.h"
#include <vector>

namespace WPEFramework {
namespace Plugin {

class UserSettingsImpl : public Exchange::IUserSettings {
public:
    UserSettingsImpl();
    ~UserSettingsImpl() override;

    uint32_t GetDisplayResolution(Resolution& resolution) override;
    uint32_t SetDisplayResolution(Resolution resolution) override;
    uint32_t GetAudioOutputEnabled(bool& enabled) override;
    uint32_t SetAudioOutputEnabled(bool enabled) override;
    void Register(INotification* notification) override;
    void Unregister(INotification* notification) override;

private:
    Resolution m_resolution;
    bool m_audioEnabled;
    std::vector<INotification*> m_notifications;

    void NotifyResolutionChanged(Resolution resolution);
    void NotifyAudioOutputChanged(bool enabled);
};

} // namespace Plugin
} // namespace WPEFramework
