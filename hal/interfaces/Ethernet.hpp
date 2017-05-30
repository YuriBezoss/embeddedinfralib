#ifndef HAL_ETHERNET_HPP
#define HAL_ETHERNET_HPP

#include "infra/util/ByteRange.hpp"
#include "infra/util/Observer.hpp"
#include <array>

namespace hal
{
    class EthernetSmi;
    class EthernetMac;

    enum class LinkSpeed
    {
        fullDuplex10MHz,
        halfDuplex10MHz,
        fullDuplex100MHz,
        halfDuplex100MHz
    };

    using MacAddress = std::array<uint8_t, 6>;

    class EthernetSmiObserver
        : public infra::SingleObserver<EthernetSmiObserver, EthernetSmi>
    {
    public:
        EthernetSmiObserver(EthernetSmi& subject);

        virtual void LinkUp(LinkSpeed linkSpeed) = 0;
        virtual void LinkDown() = 0;
    };

    // Station Management Interface deals with the communication with the Ethernet PHY
    class EthernetSmi
        : public infra::Subject<EthernetSmiObserver>
    {
    public:
        virtual uint16_t PhyAddress() const = 0;
    };

    class EthernetMacObserver
        : public infra::SingleObserver<EthernetMacObserver, EthernetMac>
    {
    public:
        EthernetMacObserver(EthernetMac& subject);

        virtual infra::ByteRange RequestReceiveBuffer() = 0;
        virtual void ReceivedFrame(uint32_t usedBuffers, uint32_t frameSize) = 0;
        virtual void ReceivedErrorFrame(uint32_t usedBuffers, uint32_t frameSize) = 0;
        virtual void SentFrame() = 0;
    };

    class EthernetMac
        : public infra::Subject<EthernetMacObserver>
    {
    public:
        virtual void SendBuffer(infra::ConstByteRange data, bool last) = 0;
        virtual void RetryAllocation() = 0;
    };
}

#endif
