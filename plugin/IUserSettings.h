/**
 * @file IUserSettings.h
 * @brief User Settings plugin COM interface
 */

#pragma once

#include <com/Ids.h>
#include <com/IIterator.h>

namespace WPEFramework {
namespace Exchange {

/**
 * @brief User Settings plugin interface
 */
struct IUserSettings : virtual public Core::IUnknown {
    enum { ID = RPC::ID_USER_SETTINGS };

    enum class Resolution {
        RESOLUTION_720P,
        RESOLUTION_1080P,
        RESOLUTION_4K
    };

    /**
     * @brief Get current display resolution
     * @param[out] resolution Current resolution
     * @return 0 on success, error code otherwise
     */
    virtual uint32_t GetDisplayResolution(Resolution& resolution) = 0;

    /**
     * @brief Set display resolution
     * @param[in] resolution Resolution to set
     * @return 0 on success, error code otherwise
     */
    virtual uint32_t SetDisplayResolution(Resolution resolution) = 0;

    /**
     * @brief New method: Get audio output configuration
     * @param[out] enabled Whether audio output is enabled
     * @return 0 on success, error code otherwise
     */
    virtual uint32_t GetAudioOutputEnabled(bool& enabled) = 0;

    /**
     * @brief New method: Set audio output configuration
     * @param[in] enabled Whether to enable audio output
     * @return 0 on success, error code otherwise
     */
    virtual uint32_t SetAudioOutputEnabled(bool enabled) = 0;

    /**
     * @brief New notification event
     */
    struct INotification : virtual public Core::IUnknown {
        enum { ID = RPC::ID_USER_SETTINGS_NOTIFICATION };

        /**
         * @brief Fired when audio output is modified
         * @param[in] enabled New audio output state
         */
        virtual void OnAudioOutputChanged(bool enabled) = 0;

        /**
         * @brief Fired when resolution changes
         * @param[in] resolution New resolution
         */
        virtual void OnResolutionChanged(Resolution resolution) = 0;
    };

    /**
     * @brief Register notification handler
     * @param[in] notification Handler to register
     */
    virtual void Register(INotification* notification) = 0;

    /**
     * @brief Unregister notification handler
     * @param[in] notification Handler to unregister
     */
    virtual void Unregister(INotification* notification) = 0;
};

} // namespace Exchange
} // namespace WPEFramework
