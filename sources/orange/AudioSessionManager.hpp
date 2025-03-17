#pragma once
#include "IAudioSession.h"
#include <memory>
#include <chrono>
#include <expected>
#include <span>
#include <cstdint>
#include <string>

namespace audio {

    class AudioSessionManager : public IAudioTrackInfo, public IAudioPlaybackControl, public IAudioEventNotifier {
    private:
        class Impl;
        std::unique_ptr<Impl> m_pImpl;

    public:
        AudioSessionManager();
        ~AudioSessionManager() override;

        [[nodiscard]] std::expected<void, std::string> initialize() noexcept;

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
