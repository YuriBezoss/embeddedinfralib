#ifndef UPGRADE_PACK_BUILDER_LIBRARY_UPGRADE_PACK_INPUT_FACTORY_HPP
#define UPGRADE_PACK_BUILDER_LIBRARY_UPGRADE_PACK_INPUT_FACTORY_HPP

#include "packs/upgrade/pack_builder/public/InputFactory.hpp"
#include "packs/upgrade/pack_builder/public/ImageSecurity.hpp"

namespace application
{
    class NoFileInput
        : public application::Input
    {
    public:
        NoFileInput(const std::string& targetName);

        virtual std::vector<uint8_t> Image() const;
    };

    class NoFileInputFactory
    {
    public:
        NoFileInputFactory(const std::string& targetName);
        NoFileInputFactory(const NoFileInputFactory& other) = delete;
        NoFileInputFactory& operator=(const NoFileInputFactory& other) = delete;
        virtual ~NoFileInputFactory() = default;

        virtual std::unique_ptr<Input> CreateInput() const = 0;

        std::string TargetName() const;

    private:
        std::string targetName;
    };

    class UpgradePackInputFactory
        : public InputFactory
    {
    public:
        UpgradePackInputFactory(const std::vector<std::string>& supportedHexTargets,
            const std::vector<std::pair<std::string, uint32_t>>& supportedBinaryTargets,
            hal::FileSystem& fileSystem, const ImageSecurity& imageSecurity, const std::vector<NoFileInputFactory*>& otherTargets);

        virtual std::unique_ptr<Input> CreateInput(const std::string& targetName, const std::string& fileName) override;

    private:
        std::vector<std::string> supportedHexTargets;
        std::vector<std::pair<std::string, uint32_t>> supportedBinaryTargets;
        hal::FileSystem& fileSystem;
        const ImageSecurity& imageSecurity;
        const std::vector<NoFileInputFactory*>& otherTargets;
    };
}

#endif
