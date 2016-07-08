#ifndef SERVICES_SYNCHRONOUS_FLASH_SPI_HPP
#define SERVICES_SYNCHRONOUS_FLASH_SPI_HPP

#include "hal/synchronous_interfaces/public/SynchronousFlashHomogeneous.hpp"
#include "hal/synchronous_interfaces/public/SynchronousSpi.hpp"

namespace services
{
    struct SynchronousFlashSpiConfig
    {
        uint32_t numberOfSubSectors = 512;
    };

    class SynchronousFlashSpi
        : public hal::SynchronousFlashHomogeneous
    {
    public:
        using Config = SynchronousFlashSpiConfig;

        static const uint8_t commandPageProgram;
        static const uint8_t commandReadData;
        static const uint8_t commandReadStatusRegister;
        static const uint8_t commandWriteEnable;
        static const uint8_t commandEraseSubSector;
        static const uint8_t commandEraseSector;
        static const uint8_t commandEraseBulk;

        static const uint32_t sizeSector = 65536;
        static const uint32_t sizeSubSector = 4096;
        static const uint32_t sizePage = 256;

        static const uint8_t statusFlagWriteInProgress = 1;

        SynchronousFlashSpi(hal::SynchronousSpi& spi, const Config& config = Config());

    public:
        virtual void WriteBuffer(infra::ConstByteRange buffer, uint32_t address) override;
        virtual void ReadBuffer(infra::ByteRange buffer, uint32_t address) override;
        virtual void EraseSectors(uint32_t beginIndex, uint32_t endIndex) override;

    private:
        std::array<uint8_t, 3> ConvertAddress(uint32_t address) const;

        void WriteEnable();
        void PageProgram();
        void EraseSomeSectors(uint32_t endIndex);
        void SendEraseSubSector(uint32_t subSectorIndex);
        void SendEraseSector(uint32_t subSectorIndex);
        void SendEraseBulk();
        void HoldWhileWriteInProgress();
        uint8_t ReadStatusRegister();

    private:
        hal::SynchronousSpi& spi;
        Config config;
        infra::ConstByteRange buffer;
        uint32_t address;
        uint32_t sectorIndex;

        struct InstructionAndAddress
        {
            uint8_t instruction;
            std::array<uint8_t, 3> address;
        } instructionAndAddress;
    };
}

#endif
