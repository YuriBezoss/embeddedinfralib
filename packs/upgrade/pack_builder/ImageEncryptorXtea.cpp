#include "mbedtls/xtea.h"
#include "packs/upgrade/pack_builder/ImageEncryptorXtea.hpp"
#include <cassert>
#include <algorithm>
#include <array>

namespace application
{
    ImageEncryptorXtea::ImageEncryptorXtea(RandomNumberGenerator& randomNumberGenerator, infra::ConstByteRange key)
        : randomNumberGenerator(randomNumberGenerator)
        , key(key)
    {}

    std::vector<uint8_t> ImageEncryptorXtea::Secure(const std::vector<uint8_t>& unalignedData) const
    {
        std::vector<uint8_t> data = unalignedData;
        data.resize(data.size() + blockLength - (data.size() - 1 + blockLength) % blockLength - 1, 0);
        std::vector<uint8_t> iv = randomNumberGenerator.Generate(blockLength);

        mbedtls2_xtea_context ctx;
        mbedtls2_xtea_init(&ctx);
        assert(key.size() == 16);
        mbedtls2_xtea_setup(&ctx, key.begin());                                                                                                          //TICS !INT#030

        std::vector<uint8_t> result = iv;
        result.resize(result.size() + data.size());

        int ret = mbedtls2_xtea_crypt_cbc(&ctx, MBEDTLS_XTEA_ENCRYPT, data.size(), iv.data(), data.data(), result.data() + blockLength);                 //TICS !INT#030
        if (ret != 0)
            throw std::exception("XTEA encryption failed");

        if (!CheckDecryption(data, result))
            throw std::exception("XTEA decryption check failed");

        return result;
    }

    bool ImageEncryptorXtea::CheckDecryption(const std::vector<uint8_t>& original, const std::vector<uint8_t>& encrypted) const
    {
        std::vector<uint8_t> iv(encrypted.begin(), encrypted.begin() + blockLength);

        mbedtls2_xtea_context ctx;
        mbedtls2_xtea_init(&ctx);
        mbedtls2_xtea_setup(&ctx, key.begin());                                                                                                          //TICS !INT#030

        if (encrypted.size() % blockLength != 0)
            return false;

        std::vector<uint8_t> decrypted(encrypted.size() - blockLength, 0);

        int ret = mbedtls2_xtea_crypt_cbc(&ctx, MBEDTLS_XTEA_DECRYPT, decrypted.size(), iv.data(), encrypted.data() + blockLength, decrypted.data());    //TICS !INT#030

        return ret == 0 && mbedtls2_xtea_self_test(0) == 0 && decrypted == original;
    }
}
