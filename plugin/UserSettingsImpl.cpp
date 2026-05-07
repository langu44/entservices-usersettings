/**
 * @file UserSettingsImpl.cpp
 * @brief User Settings plugin implementation
 */

#include "UserSettingsImpl.h"
#include <algorithm>

namespace WPEFramework {
namespace Plugin {

UserSettingsImpl::UserSettingsImpl()
    : m_resolution(Resolution::RESOLUTION_1080P)
    , m_audioEnabled(true)
{
}

UserSettingsImpl::~UserSettingsImpl() = default;

// Existing methods - unchanged
uint32_t UserSettingsImpl::GetDisplayResolution(Resolution& resolution)
{
    resolution = m_resolution;
    return 0;
}

uint32_t UserSettingsImpl::SetDisplayResolution(Resolution resolution)
{
    m_resolution = resolution;
    NotifyResolutionChanged(resolution);
    return 0;
}

// New methods - added but functionally trivial (will trigger test generation)
uint32_t UserSettingsImpl::GetAudioOutputEnabled(bool& enabled)
{
    enabled = m_audioEnabled;
    return 0;
}

uint32_t UserSettingsImpl::SetAudioOutputEnabled(bool enabled)
{
    if (m_audioEnabled != enabled) {
        m_audioEnabled = enabled;
        NotifyAudioOutputChanged(enabled);
    }
    return 0;
}

// Notification handlers
void UserSettingsImpl::Register(INotification* notification)
{
    if (notification) {
        m_notifications.push_back(notification);
    }
}

void UserSettingsImpl::Unregister(INotification* notification)
{
    auto it = std::find(m_notifications.begin(), m_notifications.end(), notification);
    if (it != m_notifications.end()) {
        m_notifications.erase(it);
    }
}

void UserSettingsImpl::NotifyResolutionChanged(Resolution resolution)
{
    for (auto notification : m_notifications) {
        notification->OnResolutionChanged(resolution);
    }
}

void UserSettingsImpl::NotifyAudioOutputChanged(bool enabled)
{
    for (auto notification : m_notifications) {
        notification->OnAudioOutputChanged(enabled);
    }
}

} // namespace Plugin
} // namespace WPEFramework
