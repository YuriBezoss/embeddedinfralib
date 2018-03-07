#include "infra/stream/LimitedInputStream.hpp"

namespace infra
{
    LimitedStreamReader::LimitedStreamReader(StreamReader& input, uint32_t length)
        : input(input)
        , length(length)
    {}

    LimitedStreamReader::LimitedStreamReader(const LimitedStreamReader& other)
        : input(other.input)
        , length(other.length)
    {}

    void LimitedStreamReader::Extract(ByteRange range, StreamErrorPolicy& errorPolicy)
    {
        errorPolicy.ReportResult(length >= range.size());
        range.shrink_from_back_to(length);
        length -= range.size();
        input.Extract(range, errorPolicy);
    }

    uint8_t LimitedStreamReader::ExtractOne(StreamErrorPolicy& errorPolicy)
    {
        if (length >= 1)
        {
            --length;
            uint8_t result = input.ExtractOne(errorPolicy);
            return result;
        }
        else
        {
            errorPolicy.ReportResult(false);

            return 0;
        }
    }

    uint8_t LimitedStreamReader::Peek(StreamErrorPolicy& errorPolicy)
    {
        errorPolicy.ReportResult(length != 0);

        if (length != 0)
            return input.Peek(errorPolicy);
        else
            return 0;
    }

    ConstByteRange LimitedStreamReader::ExtractContiguousRange(std::size_t max)
    {
        ConstByteRange result = input.ExtractContiguousRange(std::min<uint32_t>(length, max));
        length -= result.size();
        return result;
    }

    bool LimitedStreamReader::Empty() const
    {
        return length == 0 || input.Empty();
    }

    std::size_t LimitedStreamReader::Available() const
    {
        return std::min<uint32_t>(length, input.Available());
    }
}
