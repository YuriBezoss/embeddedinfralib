#ifndef HAL_SERIAL_COMMUNICATION_HPP
#define HAL_SERIAL_COMMUNICATION_HPP

#include "hal/interfaces/public/Gpio.hpp"

namespace hal
{
    class SerialCommunication
    {
    protected:
        SerialCommunication() = default;
        ~SerialCommunication() = default;

    public:
        virtual void SendData(infra::ConstByteRange data, infra::Function<void()> actionOnCompletion) = 0;
        virtual void ReceiveData(infra::Function<void(infra::ConstByteRange data)> dataReceived) = 0;
    };
}

#endif
