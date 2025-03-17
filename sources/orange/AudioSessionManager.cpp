#include "AudioSessionManager.h"
#include "WinRTAudioSession.h"
#include <memory>
#include <format>
#include "AudioSessionFactory.h" 
namespace audio {

    class AudioSessionManager::Impl {
    public:
        std::shared_ptr<platform::WinRTAudioSession> session;
        Impl() : session(std::make_shared<platform::WinRTAudioSession>()) {}
    };

    std::expected<std::shared_ptr<IAudioSession>, std::string> AudioSessionFactory::createCurrentSession() noexcept {
        try {
            auto session = std::make_shared<platform::WinRTAudioSession>();
            if (auto result = session->initialize(); !result)
                return std::unexpected(result.error());
            return session;
        }
        catch (const std::exception& ex) {
            return std::unexpected(std::format("Failed to create audio session: {}", ex.what()));
        }
    }

    AudioSessionManager::AudioSessionManager() : m_pImpl(std::make_unique<Impl>()) {}

    AudioSessionManager::~AudioSessionManager() = default;

    std::expected<void, std::string> AudioSessionManager::initialize() noexcept {
        return m_pImpl->session->initialize();
    }

    std::expected<std::chrono::seconds, std::string> AudioSessionManager::getDuration() noexcept {
        return m_pImpl->session->getDuration();
    }

    std::expected<std::chrono::seconds, std::string> AudioSessionManager::getCurrentPosition() noexcept {
        return m_pImpl->session->getCurrentPosition();
    }

    std::expected<std::string, std::string> AudioSessionManager::getTitle() const noexcept {
        return m_pImpl->session->getTitle();
    }

    std::expected<std::string, std::string> AudioSessionManager::getArtist() const noexcept {
        return m_pImpl->session->getArtist();
    }

    std::expected<std::string, std::string> AudioSessionManager::getAlbum() const noexcept {
        return m_pImpl->session->getAlbum();
    }

    std::expected<std::span<const uint8_t>, std::string> AudioSessionManager::getThumbnailBytes() noexcept {
        return m_pImpl->session->getThumbnailBytes();
    }

    std::expected<void, std::string> AudioSessionManager::play() noexcept {
        return m_pImpl->session->play();
    }

    std::expected<void, std::string> AudioSessionManager::pause() noexcept {
        return m_pImpl->session->pause();
    }

    std::expected<void, std::string> AudioSessionManager::next() noexcept {
        return m_pImpl->session->next();
    }

    std::expected<void, std::string> AudioSessionManager::previous() noexcept {
        return m_pImpl->session->previous();
    }

    std::expected<void, std::string> AudioSessionManager::seek(std::chrono::seconds position) noexcept {
        return m_pImpl->session->seek(position);
    }

    std::expected<void, std::string> AudioSessionManager::setVolume(double volume) noexcept {
        return m_pImpl->session->setVolume(volume);
    }

    std::expected<double, std::string> AudioSessionManager::getVolume() noexcept {
        return m_pImpl->session->getVolume();
    }

    void AudioSessionManager::setPlaybackChangedCallback(PlaybackChangedCallback callback) noexcept {
        m_pImpl->session->setPlaybackChangedCallback(std::move(callback));
    }

    void AudioSessionManager::setTrackChangedCallback(TrackChangedCallback callback) noexcept {
        m_pImpl->session->setTrackChangedCallback(std::move(callback));
    }

} 
