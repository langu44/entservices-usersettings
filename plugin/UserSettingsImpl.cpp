/**
 * @file UserSettingsImpl.cpp
 * @brief User Settings plugin implementation
 */

#include "IUserSettings.h"
#include <core/Singleton.h>

namespace WPEFramework {
namespace Plugin {

class UserSettingsImpl : public Exchange::IUserSettings {
public:
    UserSettingsImpl() : m_resolution(Resolution::RESOLUTION_1080P), m_audioEnabled(true) {}

    ~UserSettingsImpl() override = default;

    // Existing methods - unchanged
    uint32_t GetDisplayResolution(Resolution& resolution) override {
        resolution = m_resolution;
        return 0;
    }

    uint32_t SetDisplayResolution(Resolution resolution) override {
        m_resolution = resolution;
        NotifyResolutionChanged(resolution);
        return 0;
    }

    // New methods - added but functionally trivial (will trigger test generation)
    uint32_t GetAudioOutputEnabled(bool& enabled) override {
        enabled = m_audioEnabled;
        return 0;
    }

    uint32_t SetAudioOutputEnabled(bool enabled) override {
        if (m_audioEnabled != enabled) {
            m_audioEnabled = enabled;
            NotifyAudioOutputChanged(enabled);
        }
        return 0;
    }

    // Notification handlers
    void Register(INotification* notification) override {
        if (notification) {
            m_notifications.push_back(notification);
        }
    }

    void Unregister(INotification* notification) override {
        auto it = std::find(m_notifications.begin(), m_notifications.end(), notification);
        if (it != m_notifications.end()) {
            m_notifications.erase(it);
        }
    }

private:
    Resolution m_resolution;
    bool m_audioEnabled;
    std::vector<INotification*> m_notifications;

    void NotifyResolutionChanged(Resolution resolution) {
        for (auto notification : m_notifications) {
            notification->OnResolutionChanged(resolution);
        }
    }

    void NotifyAudioOutputChanged(bool enabled) {
        for (auto notification : m_notifications) {
            notification->OnAudioOutputChanged(enabled);
        }
    }
};

} // namespace Plugin
} // namespace WPEFramework
