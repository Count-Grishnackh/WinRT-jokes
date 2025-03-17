#include "AudioAPI.h"

namespace audio {

    AudioTrackManager::AudioTrackManager()
        : m_sessionManager(std::make_shared<AudioSessionManager>()) {
    }

    std::expected<void, std::string> AudioTrackManager::initialize() noexcept {
        return m_sessionManager->initialize();
    }

    std::expected<std::chrono::seconds, std::string> AudioTrackManager::getDuration() noexcept {
        return m_sessionManager->getDuration();
    }

    std::expected<std::chrono::seconds, std::string> AudioTrackManager::getCurrentPosition() noexcept {
        return m_sessionManager->getCurrentPosition();
    }

    std::expected<std::string, std::string> AudioTrackManager::getTitle() const noexcept {
        return m_sessionManager->getTitle();
    }

    std::expected<std::string, std::string> AudioTrackManager::getArtist() const noexcept {
        return m_sessionManager->getArtist();
    }

    std::expected<std::string, std::string> AudioTrackManager::getAlbum() const noexcept {
        return m_sessionManager->getAlbum();
    }

    std::expected<void, std::string> AudioTrackManager::play() noexcept {
        return m_sessionManager->play();
    }

    std::expected<void, std::string> AudioTrackManager::pause() noexcept {
        return m_sessionManager->pause();
    }

    std::expected<void, std::string> AudioTrackManager::next() noexcept {
        return m_sessionManager->next();
    }

    std::expected<void, std::string> AudioTrackManager::previous() noexcept {
        return m_sessionManager->previous();
    }

    std::expected<void, std::string> AudioTrackManager::seek(std::chrono::seconds position) noexcept {
        return m_sessionManager->seek(position);
    }

    std::expected<std::span<const uint8_t>, std::string> AudioTrackManager::getThumbnailBytes() noexcept {
        return m_sessionManager->getThumbnailBytes();
    }

    std::expected<void, std::string> AudioTrackManager::setVolume(double volume) noexcept {
        return m_sessionManager->setVolume(volume);
    }

    std::expected<double, std::string> AudioTrackManager::getVolume() noexcept {
        return m_sessionManager->getVolume();
    }

    void AudioTrackManager::onPlaybackStatusChanged(IAudioEventNotifier::PlaybackChangedCallback callback) {
        m_sessionManager->setPlaybackChangedCallback(std::move(callback));
    }

    void AudioTrackManager::onTrackChanged(IAudioEventNotifier::TrackChangedCallback callback) {
        m_sessionManager->setTrackChangedCallback(std::move(callback));
    }

} 