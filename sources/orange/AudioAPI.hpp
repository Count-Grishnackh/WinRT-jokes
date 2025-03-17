#pragma once
#include "AudioSessionManager.h"
#include <memory>
#include <chrono>
#include <expected>
#include <span>
#include <cstdint>
#include <string>
#include <functional>

namespace audio {

    class AudioTrackManager {
    private:
        std::shared_ptr<AudioSessionManager> m_sessionManager;

    public:
        AudioTrackManager();
        ~AudioTrackManager() = default;

        [[nodiscard]] std::expected<void, std::string> initialize() noexcept;
        [[nodiscard]] std::expected<std::chrono::seconds, std::string> getDuration() noexcept;
        [[nodiscard]] std::expected<std::chrono::seconds, std::string> getCurrentPosition() noexcept;
        [[nodiscard]] std::expected<std::string, std::string> getTitle() const noexcept;
        [[nodiscard]] std::expected<std::string, std::string> getArtist() const noexcept;
        [[nodiscard]] std::expected<std::string, std::string> getAlbum() const noexcept;
        std::expected<void, std::string> play() noexcept;
        std::expected<void, std::string> pause() noexcept;
        std::expected<void, std::string> next() noexcept;
        std::expected<void, std::string> previous() noexcept;
        std::expected<void, std::string> seek(std::chrono::seconds position) noexcept;
        [[nodiscard]] std::expected<std::span<const uint8_t>, std::string> getThumbnailBytes() noexcept;
        std::expected<void, std::string> setVolume(double volume) noexcept;
        std::expected<double, std::string> getVolume() noexcept;

        void onPlaybackStatusChanged(IAudioEventNotifier::PlaybackChangedCallback callback);
        void onTrackChanged(IAudioEventNotifier::TrackChangedCallback callback);

        template <typename Callback>
            requires std::invocable<Callback, std::string_view>
        void onPlaybackStatusChangedT(Callback&& callback) {
            onPlaybackStatusChanged(std::forward<Callback>(callback));
        }

        template <typename Callback>
            requires std::invocable<Callback, std::string_view, std::string_view>
        void onTrackChangedT(Callback&& callback) {
            onTrackChanged(std::forward<Callback>(callback));
        }
    };

}
