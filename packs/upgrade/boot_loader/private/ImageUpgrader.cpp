#include "packs/upgrade/boot_loader/public/ImageUpgrader.hpp"

namespace application
{
    ImageUpgrader::ImageUpgrader(const char* targetName, Decryptor& decryptor)
        : targetName(targetName)
        , decryptor(decryptor)
    {}

    const char* ImageUpgrader::TargetName() const
    {
        return targetName;
    }

    Decryptor& ImageUpgrader::ImageDecryptor()
    {
        return decryptor;
    }
}
