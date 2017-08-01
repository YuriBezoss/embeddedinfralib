#include <gmock/gmock.h>
#include <limits>
#include "infra/stream/StringOutputStream.hpp"
#include "services/tracer/TracerAdapterPrintf.hpp"

services::TracerAdapterPrintf* printfAdapter = nullptr;

void Print(const char* format, ...)
{
    ASSERT_TRUE(printfAdapter != nullptr);

    va_list args;

    va_start(args, format);
    printfAdapter->Print(format, args);
    va_end(args);
}

class TracerAdapterPrintfTest
    : public testing::Test
{
public:
    TracerAdapterPrintfTest()
        : tracer(stream)
        , adapter(tracer)
    {
        printfAdapter = &adapter;
    }

    infra::StringOutputStream::WithStorage<32> stream;
    services::Tracer tracer;
    services::TracerAdapterPrintf adapter;
};

TEST_F(TracerAdapterPrintfTest, print_format_string_to_tracer)
{
    Print("Hello, Tracer!");
    EXPECT_EQ("Hello, Tracer!", stream.Storage());
}

TEST_F(TracerAdapterPrintfTest, print_escaped_percent_sign_to_tracer)
{
    Print("%%");
    EXPECT_EQ("%", stream.Storage());
}

TEST_F(TracerAdapterPrintfTest, print_unknown_format_string_to_tracer)
{
    Print("%llq");
    EXPECT_EQ("llq", stream.Storage());
}

TEST_F(TracerAdapterPrintfTest, print_character_to_tracer)
{
    Print("%c", 'A');
    EXPECT_EQ("A", stream.Storage());
}

TEST_F(TracerAdapterPrintfTest, print_string_to_tracer)
{
    Print("%s", "Hello, Tracer!");
    EXPECT_EQ("Hello, Tracer!", stream.Storage());
}

TEST_F(TracerAdapterPrintfTest, print_null_string_to_tracer)
{
    Print("%s", nullptr);
    EXPECT_EQ("(null)", stream.Storage());
}

TEST_F(TracerAdapterPrintfTest, print_int_to_tracer)
{
    Print("%d", -42);
    EXPECT_EQ("-42", stream.Storage());
}

TEST_F(TracerAdapterPrintfTest, print_long_to_tracer)
{
    Print("%ld", -42L);
    EXPECT_EQ("-42", stream.Storage());
}

TEST_F(TracerAdapterPrintfTest, print_long_long_to_tracer)
{
    Print("%lld", -42LL);
    EXPECT_EQ("-42", stream.Storage());
}

TEST_F(TracerAdapterPrintfTest, print_float_to_tracer)
{
    Print("%f", 42.123f);
    EXPECT_EQ("42.123", stream.Storage());
}
