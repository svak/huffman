#include "BitsAdapter.hpp"
#include "TestsBase.hpp"

using Helpers::BitsAdapter;
using Helpers::CodeProducer;
using Helpers::bits_of;

TEST(BitsAdapter, ShouldInitalizeProperly)
{
    char buffer[4]{};
    BitsAdapter adapter{buffer, sizeof(buffer)};
    EXPECT_EQ(32u, adapter.RemainingBits());
    EXPECT_EQ(0u, adapter.BytesTaken());
}

TEST(BitsAdapter, ShouldWriteShortCode)
{
    char buffer[4]{};
    BitsAdapter adapter{buffer, sizeof(buffer)};

    adapter.Write(0b0101, 4);

    EXPECT_EQ(28u, adapter.RemainingBits());
    EXPECT_EQ(1u, adapter.BytesTaken());

    EXPECT_EQ("01010000", bits_of(buffer[0]));
    EXPECT_EQ("00000000", bits_of(buffer[1]));
    EXPECT_EQ("00000000", bits_of(buffer[2]));
}

TEST(BitsStream, TwoBytes)
{
    char buffer[4]{};
    BitsAdapter adapter{buffer, sizeof(buffer)};

    adapter.Write(0b011001101011011, 15);

    EXPECT_EQ(17u, adapter.RemainingBits());
    EXPECT_EQ(2u, adapter.BytesTaken());

    EXPECT_EQ("01100110", bits_of(buffer[0]));
    EXPECT_EQ("10110110", bits_of(buffer[1]));
    EXPECT_EQ("00000000", bits_of(buffer[2]));
}

TEST(BitsStream, SequentialWriting)
{
    char buffer[4]{};
    BitsAdapter adapter{buffer, sizeof(buffer)};

    adapter.Write(0b011, 3);
    adapter.Write(0b0011, 4);
    adapter.Write(0b0101, 4);
    adapter.Write(0b10, 2);
    adapter.Write(0b11000, 5);
    adapter.Write(0b00001, 5);

    EXPECT_EQ(9u, adapter.RemainingBits());
    EXPECT_EQ(3u, adapter.BytesTaken());

    EXPECT_EQ("01100110", bits_of(buffer[0]));
    EXPECT_EQ("10110110", bits_of(buffer[1]));
    EXPECT_EQ("00000010", bits_of(buffer[2]));
}

TEST(BitsStream, SequentialWritingWithBufferReset)
{
    char buffer[4]{};
    BitsAdapter adapter{buffer, sizeof(buffer)};

    adapter.Write(0b011011100101110111, 18);
    EXPECT_FALSE(adapter.IsFull());

    adapter.Write(0b001100110011, 12);
    EXPECT_FALSE(adapter.IsFull());

    adapter.Write(0b0101, 4);
    EXPECT_TRUE(adapter.IsFull());

    ASSERT_EQ(4u, adapter.BytesTaken());

    EXPECT_EQ("01101110", bits_of(buffer[0]));
    EXPECT_EQ("01011101", bits_of(buffer[1]));
    EXPECT_EQ("11001100", bits_of(buffer[2]));
    EXPECT_EQ("11001101", bits_of(buffer[3]));

    EXPECT_ANY_THROW(adapter.Write(0b0, 1));

    adapter.Reset();

    adapter.Write(0b10, 2);
    EXPECT_FALSE(adapter.IsFull());

    adapter.Write(0b1101, 4);
    EXPECT_FALSE(adapter.IsFull());

    ASSERT_EQ(1u, adapter.BytesTaken());

    EXPECT_EQ("01101101", bits_of(buffer[0]));
}

TEST(BitsStreamRegression, ShouldNotReproduceBugForShortCodeFromTail)
{
    char buffer[4]{};
    BitsAdapter adapter{buffer, sizeof(buffer)};

    adapter.Write(0b0, 30);
    adapter.Write(0b1111001, 7);

    EXPECT_EQ("00000011", bits_of(buffer[3]));

    adapter.Reset();

    EXPECT_EQ("11001000", bits_of(buffer[0]));
}

TEST(BitsStream, SequentialWritingLongCodes)
{
    char buffer[4]{};
    BitsAdapter adapter{buffer, sizeof(buffer)};

    adapter.Write(0b0101010101010101, 16);
    EXPECT_FALSE(adapter.IsFull());

    adapter.Write(0b10101010101011010001100111000110, 32);
    EXPECT_TRUE(adapter.IsFull());

    EXPECT_EQ("01010101", bits_of(buffer[0]));
    EXPECT_EQ("01010101", bits_of(buffer[1]));
    EXPECT_EQ("10101010", bits_of(buffer[2]));
    EXPECT_EQ("10101101", bits_of(buffer[3]));

    adapter.Reset();

    ASSERT_EQ(2u, adapter.BytesTaken());

    EXPECT_EQ("00011001", bits_of(buffer[0]));
    EXPECT_EQ("11000110", bits_of(buffer[1]));
}

TEST(CodeProducer, ShouldAcceptUpToFourBytes)
{
    CodeProducer producer;

    producer.Add(0b10101110);
    producer.Add(0b00001111);
    producer.Add(0b11110000);
    producer.Add(0b00110011);

    EXPECT_ANY_THROW(producer.Add('A'));
}

TEST(CodeProducer, ShouldEnumarateAllPossibleCodesForTwoBytes)
{
    CodeProducer producer;

    producer.Add(0xFF);
    producer.Add(0xFF);

    for (size_t i = 0; i < 16; ++i)
    {
        EXPECT_TRUE(producer.Next());
    }
    EXPECT_FALSE(producer.Next());
}

TEST(CodeProducer, ShouldEnumarateAllPossibleCodesForFourBytes)
{
    CodeProducer producer;

    producer.Add(0xFF);
    producer.Add(0xFF);
    producer.Add(0xFF);
    producer.Add(0xFF);

    for (size_t i = 0; i < 32; ++i)
    {
        EXPECT_TRUE(producer.Next());
    }
    EXPECT_FALSE(producer.Next());
}

TEST(CodeProducer, ShouldProduceCorrectCodes)
{
    CodeProducer producer;

    producer.Add(0b11010011);
    producer.Add(0b01111001);

    auto check = [&producer](const size_t aValue, const size_t aLength) {
        EXPECT_TRUE(producer.Next());
        EXPECT_EQ(aValue, producer.Value);
        EXPECT_EQ(aLength, producer.Length);
    };

    check(0b1, 1);
    check(0b11, 2);
    check(0b110, 3);
    check(0b1101, 4);
    check(0b11010, 5);
    check(0b110100, 6);
    check(0b1101001, 7);
    check(0b11010011, 8);
    check(0b110100110, 9);
    check(0b1101001101, 10);
    check(0b11010011011, 11);
    check(0b110100110111, 12);
    check(0b1101001101111, 13);
    check(0b11010011011110, 14);
    check(0b110100110111100, 15);
    check(0b1101001101111001, 16);

    EXPECT_FALSE(producer.Next());
}

TEST(CodeProducer, ShouldPreserveBitsOnRewind)
{
    CodeProducer producer;

    // Got two bytes from stream.
    producer.Add(0b11010011);
    producer.Add(0b01111001);

    // We search for valid symbol code and found it.
    for (size_t i = 0; i < 10; ++i)
    {
        producer.Next();
    }

    EXPECT_EQ(0b1101001101u, producer.Value);
    EXPECT_EQ(10u, producer.Length);

    // Now, we should cut consumed bits and rewind buffer.
    // .--------.
    // 1101001101111001........

    producer.Rewind();

    // After rewind we have:
    // 111001........

    auto check = [&producer](const size_t aValue, const size_t aLength) {
        EXPECT_TRUE(producer.Next());
        EXPECT_EQ(aValue, producer.Value);
        EXPECT_EQ(aLength, producer.Length);
    };

    // Then, we have another incoming byte.
    producer.Add(0b11001011);

    // We must attach new bits just after the existing ones:
    //       .------.
    // 11100111001011........

    check(0b1, 1);
    check(0b11, 2);
    check(0b111, 3);
    check(0b1110, 4);
    check(0b11100, 5);
    check(0b111001, 6);
    check(0b1110011, 7); // Attached begin here.
    check(0b11100111, 8);
    check(0b111001110, 9);
    check(0b1110011100, 10);
    check(0b11100111001, 11);
    check(0b111001110010, 12);
    check(0b1110011100101, 13);
    check(0b11100111001011, 14);
}

