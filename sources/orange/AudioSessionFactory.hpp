#pragma once
#include "IAudioSession.h"
#include <memory>
#include <expected>
#include <string>

namespace audio {

    class AudioSessionFactory {
    public:
        static std::expected<std::shared_ptr<IAudioSession>, std::string> createCurrentSession() noexcept;
    };

} 