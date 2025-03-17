#pragma once
#include "IAudioSession.h"
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Media.Playback.h>
#include <mutex>
#include <optional>
#include <vector>
#include <chrono>
#include <span>
#include <expected>
#include <cstdint>
#include <string>

namespace audio {
    namespace platform {

        class WinRTAudioSession : public IAudioSession {
        private:
            winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionManager m_sessionManager{ nullptr };
            winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession m_currentSession{ nullptr };

            struct Cache {
                std::mutex mutex;
                std::optional<winrt::Windows::Storage::Streams::IRandomAccessStreamReference> thumbnailRef;
                std::optional<std::vector<uint8_t>> thumbnailBytes;
                std::optional<std::chrono::seconds> duration;
            };

            Cache m_cache;
            winrt::event_token m_playbackChangedToken{};
            winrt::event_token m_mediaPropertiesChangedToken{};

            std::expected<winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionPlaybackInfo, std::string>
                getPlaybackInfo() const noexcept;

            std::expected<winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionMediaProperties, std::string>
                getMediaProperties() const noexcept;

            void clearCache() noexcept;

        public:
            WinRTAudioSession() = default;
            ~WinRTAudioSession() noexcept override;
            WinRTAudioSession(const WinRTAudioSession&) = delete;
            WinRTAudioSession& operator=(const WinRTAudioSession&) = delete;
            WinRTAudioSession(WinRTAudioSession&&) noexcept = default;
            WinRTAudioSession& operator=(WinRTAudioSession&&) noexcept = default;

            std::expected<void, std::string> initialize() noexcept override;
            std::expected<std::chrono::seconds, std::string> getDuration() noexcept override;
            std::expected<std::chrono::seconds, std::string> getCurrentPosition() noexcept override;
            std::expected<std::string, std::string> getTitle() const noexcept override;
            std::expected<std::string, std::string> getArtist() const noexcept override;
            std::expected<std::string, std::string> getAlbum() const noexcept override;
            std::expected<std::span<const uint8_t>, std::string> getThumbnailBytes() noexcept override;

            std::expected<void, std::string> play() noexcept override;
            std::expected<void, std::string> pause() noexcept override;
            std::expected<void, std::string> next() noexcept override;
            std::expected<void, std::string> previous() noexcept override;
            std::expected<void, std::string> seek(std::chrono::seconds position) noexcept override;
            std::expected<void, std::string> setVolume(double volume) noexcept override;
            std::expected<double, std::string> getVolume() noexcept override;

            void setPlaybackChangedCallback(PlaybackChangedCallback callback) noexcept override;
            void setTrackChangedCallback(TrackChangedCallback callback) noexcept override;
        };

    } 
}
