#include <memory>
#include <chrono>
#include <functional>
#include <string>
#include <cstring>
#include "AudioAPI.h"
#include "AudioSessionManager.h"

#if defined(_WIN32) || defined(_WIN64)
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT __attribute__((visibility("default")))
#endif

struct ExpectedResult {
    bool has_value;
    void* value_or_error;
};

template<typename T>
ExpectedResult makeSuccess(T value) {
    T* ptr = new T(value);
    return { true, ptr };
}

ExpectedResult makeVoidSuccess() {
    return { true, nullptr };
}

ExpectedResult makeError(const std::string& error) {
    char* err_str = new char[error.length() + 1];
#ifdef _MSC_VER
    strcpy_s(err_str, error.length() + 1, error.c_str());
#else
    strncpy(err_str, error.c_str(), error.length() + 1);
    err_str[error.length()] = '\0'; 
#endif
    return { false, err_str };
}

using PlaybackCallback = void (*)(const char*);
using TrackChangedCallback = void (*)(const char*, const char*);
void safeCopyString(char* dest, size_t destSize, const char* src) {
#ifdef _MSC_VER
    strcpy_s(dest, destSize, src);
#else
    strncpy(dest, src, destSize - 1);
    dest[destSize - 1] = '\0'; 
#endif
}

extern "C" {
    API_EXPORT void* createAudioManager() {
        try {
            return new audio::AudioTrackManager();
        }
        catch (const std::exception&) {
            return nullptr;
        }
    }

    API_EXPORT void destroyAudioManager(void* managerPtr) {
        if (managerPtr) {
            auto* manager = static_cast<audio::AudioTrackManager*>(managerPtr);
            delete manager;
        }
    }

    API_EXPORT ExpectedResult initialize(void* managerPtr) {
        if (!managerPtr) return makeError("Invalid manager pointer");

        auto* manager = static_cast<audio::AudioTrackManager*>(managerPtr);

        auto result = manager->initialize();

        if (result) {
            return makeVoidSuccess();
        }
        else {
            return makeError(result.error());
        }
    }

    API_EXPORT ExpectedResult getDuration(void* managerPtr) {
        if (!managerPtr) return makeError("Invalid manager pointer");

        auto* manager = static_cast<audio::AudioTrackManager*>(managerPtr);
        auto result = manager->getDuration();

        if (result) {
            auto* seconds = new int64_t(result.value().count());
            return { true, seconds };
        }
        else {
            return makeError(result.error());
        }
    }

    API_EXPORT ExpectedResult getCurrentPosition(void* managerPtr) {
        if (!managerPtr) return makeError("Invalid manager pointer");

        auto* manager = static_cast<audio::AudioTrackManager*>(managerPtr);
        auto result = manager->getCurrentPosition();

        if (result) {
            auto* seconds = new int64_t(result.value().count());
            return { true, seconds };
        }
        else {
            return makeError(result.error());
        }
    }

    API_EXPORT ExpectedResult getTitle(void* managerPtr) {
        if (!managerPtr) return makeError("Invalid manager pointer");

        auto* manager = static_cast<audio::AudioTrackManager*>(managerPtr);
        auto result = manager->getTitle();

        if (result) {
            const std::string& title = result.value();
            char* title_str = new char[title.length() + 1];
            safeCopyString(title_str, title.length() + 1, title.c_str());
            return { true, title_str };
        }
        else {
            return makeError(result.error());
        }
    }

    API_EXPORT ExpectedResult getArtist(void* managerPtr) {
        if (!managerPtr) return makeError("Invalid manager pointer");

        auto* manager = static_cast<audio::AudioTrackManager*>(managerPtr);
        auto result = manager->getArtist();

        if (result) {
            const std::string& artist = result.value();
            char* artist_str = new char[artist.length() + 1];
            safeCopyString(artist_str, artist.length() + 1, artist.c_str());
            return { true, artist_str };
        }
        else {
            return makeError(result.error());
        }
    }

    API_EXPORT ExpectedResult getAlbum(void* managerPtr) {
        if (!managerPtr) return makeError("Invalid manager pointer");

        auto* manager = static_cast<audio::AudioTrackManager*>(managerPtr);
        auto result = manager->getAlbum();

        if (result) {
            const std::string& album = result.value();
            char* album_str = new char[album.length() + 1];
            safeCopyString(album_str, album.length() + 1, album.c_str());
            return { true, album_str };
        }
        else {
            return makeError(result.error());
        }
    }

    API_EXPORT ExpectedResult play(void* managerPtr) {
        if (!managerPtr) return makeError("Invalid manager pointer");

        auto* manager = static_cast<audio::AudioTrackManager*>(managerPtr);
        auto result = manager->play();

        if (result) {
            return makeVoidSuccess();
        }
        else {
            return makeError(result.error());
        }
    }

    API_EXPORT ExpectedResult pause(void* managerPtr) {
        if (!managerPtr) return makeError("Invalid manager pointer");

        auto* manager = static_cast<audio::AudioTrackManager*>(managerPtr);
        auto result = manager->pause();

        if (result) {
            return makeVoidSuccess();
        }
        else {
            return makeError(result.error());
        }
    }

    API_EXPORT ExpectedResult next(void* managerPtr) {
        if (!managerPtr) return makeError("Invalid manager pointer");

        auto* manager = static_cast<audio::AudioTrackManager*>(managerPtr);
        auto result = manager->next();

        if (result) {
            return makeVoidSuccess();
        }
        else {
            return makeError(result.error());
        }
    }

    API_EXPORT ExpectedResult previous(void* managerPtr) {
        if (!managerPtr) return makeError("Invalid manager pointer");

        auto* manager = static_cast<audio::AudioTrackManager*>(managerPtr);
        auto result = manager->previous();

        if (result) {
            return makeVoidSuccess();
        }
        else {
            return makeError(result.error());
        }
    }

    API_EXPORT ExpectedResult seek(void* managerPtr, int64_t seconds) {
        if (!managerPtr) return makeError("Invalid manager pointer");

        auto* manager = static_cast<audio::AudioTrackManager*>(managerPtr);



        auto result = manager->seek(std::chrono::seconds(seconds));

        if (result) {
            return makeVoidSuccess();
        }
        else {
            return makeError(result.error());
        }
    }
    API_EXPORT ExpectedResult setVolume(void* managerPtr, double volume) {
        if (!managerPtr) return makeError("Invalid manager pointer");

        auto* manager = static_cast<audio::AudioTrackManager*>(managerPtr);
        auto result = manager->setVolume(volume);

        if (result) {
            return makeVoidSuccess();
        }
        else {
            return makeError(result.error());
        }
    }

    API_EXPORT ExpectedResult getVolume(void* managerPtr) {
        if (!managerPtr) return makeError("Invalid manager pointer");

        auto* manager = static_cast<audio::AudioTrackManager*>(managerPtr);
        auto result = manager->getVolume();

        if (result) {
            auto* volume = new double(result.value());
            return { true, volume };
        }
        else {
            return makeError(result.error());
        }
    }

    API_EXPORT ExpectedResult getThumbnailBytes(void* managerPtr, uint8_t** outBuffer, size_t* outSize) {
        if (!managerPtr || !outBuffer || !outSize) return makeError("Invalid pointers");

        auto* manager = static_cast<audio::AudioTrackManager*>(managerPtr);

 
        auto result = manager->getThumbnailBytes();

        if (result) {
            auto span = result.value();
            size_t size = span.size();
            *outSize = size;
            uint8_t* buffer = new uint8_t[size];
            std::memcpy(buffer, span.data(), size);
            *outBuffer = buffer;

            return makeVoidSuccess();
        }
        else {
            return makeError(result.error());
        }
    }

    API_EXPORT void setPlaybackCallback(void* managerPtr, PlaybackCallback callback) {
        if (!managerPtr || !callback) return;

        auto* manager = static_cast<audio::AudioTrackManager*>(managerPtr);

        manager->onPlaybackStatusChangedT([callback](std::string_view status) {
            std::string status_copy(status);
            callback(status_copy.c_str());
            });
    }

    API_EXPORT void setTrackCallback(void* managerPtr, TrackChangedCallback callback) {
        if (!managerPtr || !callback) return;

        auto* manager = static_cast<audio::AudioTrackManager*>(managerPtr);

        manager->onTrackChangedT([callback](std::string_view title, std::string_view artist) {
            std::string title_copy(title);
            std::string artist_copy(artist);
            callback(title_copy.c_str(), artist_copy.c_str());
            });
    }

    API_EXPORT void freeString(char* str) {
        delete[] str;
    }

    API_EXPORT void freeInt64(int64_t* ptr) {
        delete ptr;
    }

    API_EXPORT void freeDouble(double* ptr) {
        delete ptr;
    }

    API_EXPORT void freeThumbnailData(uint8_t* buffer) {
        delete[] buffer;
    }

}