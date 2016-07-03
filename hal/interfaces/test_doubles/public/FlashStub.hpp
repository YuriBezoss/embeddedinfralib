#ifndef HAL_STUB_FLASH_STUB_HPP
#define HAL_STUB_FLASH_STUB_HPP

#include "hal/interfaces/public/Flash.hpp"
#include "infra/util/public/Optional.hpp"

namespace hal
{
    class FlashStub
        : public hal::Flash
    {
    public:
        FlashStub(uint32_t numberOfSectors, uint32_t sizeOfEachSector);

        void Clear();

        virtual uint32_t NumberOfSectors() const override;
        virtual uint32_t SizeOfSector(uint32_t sectorIndex) const override;

        virtual uint32_t SectorOfAddress(uint32_t address) const override;
        virtual uint32_t AddressOfSector(uint32_t sectorIndex) const override;

        virtual void WriteBuffer(infra::ConstByteRange buffer, uint32_t address, infra::Function<void()> onDone) override;
        virtual void ReadBuffer(infra::ByteRange buffer, uint32_t address, infra::Function<void()> onDone) override;
        virtual void EraseSectors(uint32_t beginIndex, uint32_t endIndex, infra::Function<void()> onDone) override;

    public:
        std::vector<std::vector<uint8_t>> sectors;
        infra::Optional<uint8_t> stopAfterWriteSteps;

        infra::Function<void()> onEraseDone;
        bool delaySignalEraseDone = false;
    };
}

#endif
