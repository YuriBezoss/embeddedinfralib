#ifndef UPGRADE_PACK_BUILD_LIBRARY_ENCRYPTOR_NONE_HPP
#define UPGRADE_PACK_BUILD_LIBRARY_ENCRYPTOR_NONE_HPP

#include "packs/upgrade/pack_builder/public/ImageSecurity.hpp"
#include <cstdint>
#include <vector>

namespace application
{
    class ImageEncryptorNone
        : public ImageSecurity
    {
    public:
        static const uint32_t encryptionAndMacMethod = 0;
        static const std::size_t blockLength = 16;

        virtual uint32_t EncryptionAndMacMethod() const override;
        virtual std::vector<uint8_t> Secure(const std::vector<uint8_t>& data) const override;
    };
}

#endif
