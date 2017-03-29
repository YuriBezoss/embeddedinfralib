#ifndef SERVICES_CYCLIC_STORE_HPP
#define SERVICES_CYCLIC_STORE_HPP

#include "hal/interfaces/public/Flash.hpp"
#include "infra/event/public/ClaimableResource.hpp"
#include "infra/util/public/IntrusiveForwardList.hpp"
#include "infra/util/public/Sequencer.hpp"

namespace services
{
    class CyclicStore
    {
    public:
        explicit CyclicStore(hal::Flash& flash);
        CyclicStore(const CyclicStore& other) = delete;
        CyclicStore& operator=(const CyclicStore& other) = delete;

        void Add(infra::ConstByteRange range, infra::Function<void()> onDone);
        void AddPartial(infra::ConstByteRange range, uint32_t totalSize, infra::Function<void()> onDone);
        void Clear(infra::Function<void()> onDone);

        class Iterator;

        Iterator Begin() const;

    private:
        void Recover();
        void RecoverSector(uint32_t sectorIndex);
        void RecoverEndAddress();

        void EraseSectorIfAtStart();
        void FillSectorIfDataDoesNotFit(std::size_t size);
        void WriteSectorStatusIfAtStartOfSector();
        void WriteRange(infra::ConstByteRange range);

    private:
        hal::Flash& flash;
        uint32_t startAddress;
        uint32_t endAddress;
        infra::Sequencer sequencer;
        uint32_t sectorIndex;
        mutable infra::ClaimableResource resource;
        infra::ClaimableResource::Claimer::WithSize<12> claimer;
        infra::ClaimableResource::Claimer::WithSize<12> recoverClaimer;
        infra::AutoResetFunction<void()> onAddDone;
        infra::AutoResetFunction<void()> onClearDone;
        uint32_t remainingPartialSize = 0;
        uint32_t partialSizeWritten = 0;
        bool partialAddStarted = false;

        using Length = uint16_t;

        enum class BlockStatus: uint8_t
        {
            empty = 0xff,
            emptyUntilEnd = 0x7f,
            writingLength = 0xfe,
            writingData = 0xfc,
            dataReady = 0xf8
        };

        enum class SectorStatus: uint8_t
        {
            empty = 0xff,
            used = 0xfe,
            firstInCycle = 0xfc
        };

        enum class RecoverPhase: uint8_t
        {
            searchingStartOrEmpty,
            checkingAllUsed,
            checkingUsedFollowedByEmpty,
            checkingAllEmpty,
            checkingAllUsedOrAllEmpty,
            corrupt,
            done
        };

        struct BlockHeader
        {
            BlockStatus status = BlockStatus::empty;
            uint8_t lengthLsb = 0;
            uint8_t lengthMsb = 0;

            Length BlockLength() const
            {
                return lengthLsb + static_cast<Length>(lengthMsb << 8);
            }

            void SetBlockLength(Length length)
            {
                lengthLsb = static_cast<uint8_t>(length);
                lengthMsb = static_cast<uint8_t>(length >> 8);
            }
        };

        SectorStatus sectorStatus = SectorStatus::empty;
        BlockHeader blockHeader;
        RecoverPhase recoverPhase = RecoverPhase::searchingStartOrEmpty;

        mutable infra::IntrusiveForwardList<Iterator> iterators;
    };

    class CyclicStore::Iterator
        : public infra::IntrusiveForwardList<Iterator>::NodeType
    {
    public:
        explicit Iterator(const CyclicStore& store);
        Iterator(const Iterator& other);
        ~Iterator();
        Iterator& operator=(const Iterator& other);

        void Read(infra::ByteRange buffer, infra::Function<void(infra::ByteRange result)> onDone);

        void SectorIsErased(uint32_t sectorIndex);

    private:
        void ReadSectorStatusIfAtStart();

    private:
        const CyclicStore& store;
        bool loadStartAddressDelayed;
        uint32_t address;
        infra::Sequencer sequencer;
        infra::ClaimableResource::Claimer::WithSize<INFRA_DEFAULT_FUNCTION_EXTRA_SIZE + 8> claimer;

        SectorStatus sectorStatus = SectorStatus::used;
        bool firstSectorToRead = true;
        BlockHeader blockHeader;
        infra::ByteRange readBuffer;

        bool found = false;
    };
}

#endif
