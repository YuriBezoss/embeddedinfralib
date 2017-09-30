#include "gmock/gmock.h"
#include "infra/stream/ByteOutputStream.hpp"
#include "protobuf/echo/ProtoFormatter.hpp"

TEST(ProtoFormatterTest, PutVarInt_as_single_byte)
{
    infra::ByteOutputStream::WithStorage<20> stream;
    services::ProtoFormatter formatter(stream);

    formatter.PutVarIntField(2, 4);
    EXPECT_EQ((std::array<uint8_t, 2>{ 4 << 3, 2 }), stream.Writer().Processed());
}

TEST(ProtoFormatterTest, PutVarInt_as_multiple_bytes)
{
    infra::ByteOutputStream::WithStorage<20> stream;
    services::ProtoFormatter formatter(stream);

    formatter.PutVarIntField(389, 4);
    EXPECT_EQ((std::array<uint8_t, 3>{ 4 << 3, 0x85, 3 }), stream.Writer().Processed());
}

TEST(ProtoFormatterTest, negative_int_encodes_as_10_bytes)
{
    infra::ByteOutputStream::WithStorage<20> stream;
    services::ProtoFormatter formatter(stream);

    formatter.PutVarIntField(-1, 4);
    EXPECT_EQ((std::array<uint8_t, 11>{ 4 << 3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 1 }), stream.Writer().Processed());
}

TEST(ProtoFormatterTest, negative_signed_int_encodes_as_1_byte)
{
    infra::ByteOutputStream::WithStorage<20> stream;
    services::ProtoFormatter formatter(stream);

    formatter.PutSignedVarIntField(-1, 4);
    EXPECT_EQ((std::array<uint8_t, 2>{ 4 << 3, 1 }), stream.Writer().Processed());
}

TEST(ProtoFormatterTest, PutFixed32)
{
    infra::ByteOutputStream::WithStorage<20> stream;
    services::ProtoFormatter formatter(stream);

    formatter.PutFixed32Field(2, 4);
    EXPECT_EQ((std::array<uint8_t, 5>{ 4 << 3, 2, 0, 0, 0 }), stream.Writer().Processed());
}

TEST(ProtoFormatterTest, PutFixed64)
{
    infra::ByteOutputStream::WithStorage<20> stream;
    services::ProtoFormatter formatter(stream);

    formatter.PutFixed64Field(2, 4);
    EXPECT_EQ((std::array<uint8_t, 9>{ 4 << 3, 2, 0, 0, 0, 0, 0, 0, 0 }), stream.Writer().Processed());
}

TEST(ProtoFormatterTest, PutStringField)
{
    infra::ByteOutputStream::WithStorage<20> stream;
    services::ProtoFormatter formatter(stream);

    formatter.PutStringField("a", 4);
    EXPECT_EQ((std::array<uint8_t, 3>{ 4 << 3 | 2, 1, 'a' }), stream.Writer().Processed());
}

TEST(ProtoFormatterTest, PutSubObject)
{
    infra::ByteOutputStream::WithStorage<20> stream;
    services::ProtoFormatter formatter(stream);

    {
        services::ProtoLengthDelimitedFormatter subObjectFormatter = formatter.LengthDelimitedFormatter(4);
        formatter.PutVarIntField(2, 4);
    }

    EXPECT_EQ((std::array<uint8_t, 4>{ 4 << 3 | 2, 2, 4 << 3, 2 }), stream.Writer().Processed());
}
