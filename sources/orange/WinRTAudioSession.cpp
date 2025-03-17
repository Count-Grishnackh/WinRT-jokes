#include "WinRTAudioSession.h"
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Media.Core.h>
#include <winrt/Windows.Media.Playback.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/base.h>
#include <format>
#include <vector>
#include <algorithm>

namespace audio {
	namespace platform {

			WinRTAudioSession::~WinRTAudioSession() noexcept {
			if (m_currentSession) {
				if (m_playbackChangedToken)
					m_currentSession.PlaybackInfoChanged(m_playbackChangedToken);
				if (m_mediaPropertiesChangedToken)
					m_currentSession.MediaPropertiesChanged(m_mediaPropertiesChangedToken);
			}
		}

		std::expected<void, std::string> WinRTAudioSession::initialize() noexcept {
			try {
				auto asyncManager = winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionManager::RequestAsync();
				m_sessionManager = asyncManager.get();
				winrt::Windows::Foundation::Collections::IVectorView<winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession> sessions = m_sessionManager.GetSessions();
				if (sessions.Size() > 0) {
					m_currentSession = sessions.GetAt(0);
				}
				else {
					return std::unexpected("No active audio session found.");
				}
				return {};
			}
			catch (const std::exception& ex) {
				return std::unexpected(std::format("Initialization failed: {}", ex.what()));
			}
		}

#if _WIN32_WINNT >= 0x0A00
		auto getTimelineProperties(winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession const& session)
			-> winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionTimelineProperties
		{
			return session.GetTimelinePropertiesAsync().get();
		}
#endif

		std::expected<std::chrono::seconds, std::string> WinRTAudioSession::getDuration() noexcept {
			if (!m_currentSession)
				return std::unexpected("No active session.");
			try {
#if _WIN32_WINNT >= 0x0A00
				auto timeline = getTimelineProperties(m_currentSession);
				auto duration = timeline.EndTime() - timeline.StartTime();
				int64_t seconds = duration.count() / 10000000LL;
				return std::chrono::seconds(seconds);
#else
				return std::unexpected("Timeline properties not supported on this platform.");
#endif
			}
			catch (const std::exception& ex) {
				return std::unexpected(std::format("Error getting duration: {}", ex.what()));
			}
		}

		std::expected<std::chrono::seconds, std::string> WinRTAudioSession::getCurrentPosition() noexcept {
			if (!m_currentSession)
				return std::unexpected("No active session.");
			try {
#if _WIN32_WINNT >= 0x0A00
				auto timeline = getTimelineProperties(m_currentSession);
				int64_t seconds = timeline.Position().count() / 10000000LL;
				return std::chrono::seconds(seconds);
#else
				return std::unexpected("Timeline properties not supported on this platform.");
#endif
			}
			catch (const std::exception& ex) {
				return std::unexpected(std::format("Error getting current position: {}", ex.what()));
			}
		}

		std::expected<std::string, std::string> WinRTAudioSession::getTitle() const noexcept {
			if (!m_currentSession)
				return std::unexpected("No active session.");
			try {
				auto mediaProps = m_currentSession.TryGetMediaPropertiesAsync().get();
				return winrt::to_string(mediaProps.Title());
			}
			catch (const std::exception& ex) {
				return std::unexpected(std::format("Error getting title: {}", ex.what()));
			}
		}

		std::expected<std::string, std::string> WinRTAudioSession::getArtist() const noexcept {
			if (!m_currentSession)
				return std::unexpected("No active session.");
			try {
				auto mediaProps = m_currentSession.TryGetMediaPropertiesAsync().get();
				return winrt::to_string(mediaProps.Artist());
			}
			catch (const std::exception& ex) {
				return std::unexpected(std::format("Error getting artist: {}", ex.what()));
			}
		}

		std::expected<std::string, std::string> WinRTAudioSession::getAlbum() const noexcept {
			if (!m_currentSession)
				return std::unexpected("No active session.");
			try {
				auto mediaProps = m_currentSession.TryGetMediaPropertiesAsync().get();
				return winrt::to_string(mediaProps.AlbumTitle());
			}
			catch (const std::exception& ex) {
				return std::unexpected(std::format("Error getting album: {}", ex.what()));
			}
		}

		std::expected<std::span<const uint8_t>, std::string> WinRTAudioSession::getThumbnailBytes() noexcept {
			if (!m_currentSession)
				return std::unexpected("No active session.");
			try {
				auto mediaProps = m_currentSession.TryGetMediaPropertiesAsync().get();
				auto thumbnail = mediaProps.Thumbnail();
				if (!thumbnail)
					return std::unexpected("No thumbnail available.");

				auto stream = thumbnail.OpenReadAsync().get();
				uint64_t size = stream.Size();
				winrt::Windows::Storage::Streams::Buffer bufferWinRT{ static_cast<uint32_t>(size) };
				auto readOp = stream.ReadAsync(bufferWinRT, bufferWinRT.Capacity(), winrt::Windows::Storage::Streams::InputStreamOptions::None);
				readOp.get();

				std::vector<uint8_t> buffer(bufferWinRT.Length());
				std::copy_n(bufferWinRT.data(), bufferWinRT.Length(), buffer.begin());

				m_cache.thumbnailBytes = std::move(buffer);
				return std::span<const uint8_t>(m_cache.thumbnailBytes->data(), m_cache.thumbnailBytes->size());
			}
			catch (const std::exception& ex) {
				return std::unexpected(std::format("Error getting thumbnail: {}", ex.what()));
			}
		}

		std::expected<void, std::string> WinRTAudioSession::play() noexcept {
			if (!m_currentSession)
				return std::unexpected("No active session.");
			try {
				m_currentSession.TryPlayAsync().get();
				return {};
			}
			catch (const std::exception& ex) {
				return std::unexpected(std::format("Play failed: {}", ex.what()));
			}
		}

		std::expected<void, std::string> WinRTAudioSession::pause() noexcept {
			if (!m_currentSession)
				return std::unexpected("No active session.");
			try {
				m_currentSession.TryPauseAsync().get();
				return {};
			}
			catch (const std::exception& ex) {
				return std::unexpected(std::format("Pause failed: {}", ex.what()));
			}
		}

		std::expected<void, std::string> WinRTAudioSession::next() noexcept {
			if (!m_currentSession)
				return std::unexpected("No active session.");
			try {
				m_currentSession.TrySkipNextAsync().get();
				return {};
			}
			catch (const std::exception& ex) {
				return std::unexpected(std::format("Next track failed: {}", ex.what()));
			}
		}

		std::expected<void, std::string> WinRTAudioSession::previous() noexcept {
			if (!m_currentSession)
				return std::unexpected("No active session.");
			try {
				m_currentSession.TrySkipPreviousAsync().get();
				return {};
			}
			catch (const std::exception& ex) {
				return std::unexpected(std::format("Previous track failed: {}", ex.what()));
			}
		}

		std::expected<void, std::string> WinRTAudioSession::seek(std::chrono::seconds position) noexcept {
			return std::unexpected("Seek operation is not supported by the current session.");
		}

		std::expected<void, std::string> WinRTAudioSession::setVolume(double volume) noexcept {
			return std::unexpected("Volume control is not supported by the current session.");
		}

		std::expected<double, std::string> WinRTAudioSession::getVolume() noexcept {
			return std::unexpected("Volume control is not supported by the current session.");
		}

		void WinRTAudioSession::setPlaybackChangedCallback(PlaybackChangedCallback callback) noexcept {
			if (!m_currentSession)
				return;
			m_playbackChangedToken = m_currentSession.PlaybackInfoChanged([callback](auto const&, auto const&) {
				callback("Playback info updated");
				});
		}

		void WinRTAudioSession::setTrackChangedCallback(TrackChangedCallback callback) noexcept {
			if (!m_currentSession)
				return;
			m_mediaPropertiesChangedToken = m_currentSession.MediaPropertiesChanged([this, callback](auto const&, auto const&) {
				try {
					auto mediaProps = m_currentSession.TryGetMediaPropertiesAsync().get();
					callback(winrt::to_string(mediaProps.Title()),
						winrt::to_string(mediaProps.Artist()));
				}
				catch (...) {
					callback("Unknown", "Unknown");
				}
				});
		}

		std::expected<winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionPlaybackInfo, std::string>
			WinRTAudioSession::getPlaybackInfo() const noexcept {
			if (!m_currentSession)
				return std::unexpected("No active session.");
			try {
				auto playbackInfo = m_currentSession.GetPlaybackInfo();
				return playbackInfo;
			}
			catch (const std::exception& ex) {
				return std::unexpected(std::format("Error getting playback info: {}", ex.what()));
			}
		}

		std::expected<winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionMediaProperties, std::string>
			WinRTAudioSession::getMediaProperties() const noexcept {
			if (!m_currentSession)
				return std::unexpected("No active session.");
			try {
				return m_currentSession.TryGetMediaPropertiesAsync().get();
			}
			catch (const std::exception& ex) {
				return std::unexpected(std::format("Error getting media properties: {}", ex.what()));
			}
		}

		void WinRTAudioSession::clearCache() noexcept {
			std::scoped_lock lock(m_cache.mutex);
			m_cache.thumbnailBytes.reset();
			m_cache.thumbnailRef.reset();
			m_cache.duration.reset();
		}

	} 
} 
