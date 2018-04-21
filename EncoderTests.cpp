#include "Interfaces.hpp"
#include "Processor.hpp"
#include "TestsBase.hpp"

using Helpers::bits_of;
using Model::FileHeader;

struct InputFileMock : IStorage::Input
{
    MOCK_METHOD1(ReadTo, bool(IStorage::Input::TBuffer* const));
    MOCK_METHOD2(Read, size_t(char*, size_t));
    MOCK_METHOD0(Reset, void());
};

struct OutputFileMock : IStorage::Output
{
    MOCK_METHOD2(Write, void(const char*, size_t));
    MOCK_METHOD1(Skip, void(size_t));
    MOCK_METHOD0(Reset, void());
};

TEST(Encoder, ShouldWriteEmptyTableForEmptySource)
{
    // Given input and output file.
    InputFileMock input;
    OutputFileMock output;

    // We expect that header will be written,
    EXPECT_CALL(output, Skip(Eq(sizeof(FileHeader)))).Times(1);
    EXPECT_CALL(output, Reset()).Times(1);

    EXPECT_CALL(output, Write(_, _)).WillOnce(Invoke([](const char* aData, size_t aSize) {
        EXPECT_EQ(sizeof(FileHeader), aSize);

        auto header = reinterpret_cast<const FileHeader*>(aData);
        EXPECT_EQ(0, header->FileSize);
        EXPECT_EQ(0, header->Table.Length);
    }));

    // When we encode empty file.
    EXPECT_CALL(input, ReadTo(_)).Times(2).WillRepeatedly(Return(false));
    EXPECT_CALL(input, Reset()).Times(1);

    Processor::Encode(input, output);
}

TEST(Encoder, ShouldEncodeOneSymbolSourceFile)
{
    // Given non-empty input.
    InputFileMock input;
    OutputFileMock output;

    // We expect that we left space for header.
    EXPECT_CALL(output, Skip(Eq(sizeof(FileHeader)))).Times(1);
    EXPECT_CALL(output, Reset()).Times(1);

    // Write symbols table, data and update header.
    EXPECT_CALL(output, Write(_, _))
        .WillOnce(Invoke([](const char* aData, size_t aSize) {
            EXPECT_EQ(sizeof(FileHeader::SymbolsTable::Entry), aSize);
            auto entry = reinterpret_cast<const FileHeader::SymbolsTable::Entry*>(aData);
            EXPECT_EQ('A', entry->Symbol);
            EXPECT_EQ(0b0, entry->Code.Value);
            EXPECT_EQ(0b1, entry->Code.Length);
        }))
        .WillOnce(Invoke([](const char* aData, size_t aSize) {
            EXPECT_EQ(1, aSize);
            auto data = reinterpret_cast<const uint8_t*>(aData);
            EXPECT_EQ("00000000", bits_of(*data));
        }))
        .WillOnce(Invoke([](const char* aData, size_t aSize) {
            EXPECT_EQ(sizeof(FileHeader), aSize);
            auto header = reinterpret_cast<const FileHeader*>(aData);
            EXPECT_EQ(1, header->FileSize);
            EXPECT_EQ(1, header->Table.Length);
        }));

    // When we encode one-byte file.
    EXPECT_CALL(input, Reset()).Times(1);

    EXPECT_CALL(input, ReadTo(_))
        .WillOnce(Invoke([](auto* const aBuffer) {
            std::string data = "A";
            aBuffer->assign(data.begin(), data.end());
            return true;
        }))
        .WillOnce(Return(false))
        .WillOnce(Invoke([](auto* const aBuffer) {
            std::string data = "A";
            aBuffer->assign(data.begin(), data.end());
            return true;
        }))
        .WillOnce(Return(false));

    Processor::Encode(input, output);
}

TEST(Encoder, ShouldEncodeSimpleStream)
{
    // Given sources.
    InputFileMock input;
    OutputFileMock output;

    // We expect that symbols will be encoded and written.
    EXPECT_CALL(output, Skip(Eq(sizeof(FileHeader)))).Times(1);
    EXPECT_CALL(output, Reset()).Times(1);

    EXPECT_CALL(output, Write(_, _))
        .WillOnce(Invoke([](const char*, size_t) {}))
        .WillOnce(Invoke([](const char*, size_t) {}))
        .WillOnce(Invoke([](const char*, size_t) {}))
        .WillOnce(Invoke([](const char* aData, size_t aSize) {
            EXPECT_EQ(2, aSize);
            auto data = reinterpret_cast<const uint8_t*>(aData);
            EXPECT_EQ("00011111", bits_of(*data++));
            EXPECT_EQ("00000000", bits_of(*data++));
        }))
        .WillOnce(Invoke([](const char* aData, size_t aSize) {
            EXPECT_EQ(sizeof(FileHeader), aSize);
            auto header = reinterpret_cast<const FileHeader*>(aData);
            EXPECT_EQ(6, header->FileSize);
            EXPECT_EQ(3, header->Table.Length);
        }));

    // When we encode AAABBC sequence.
    EXPECT_CALL(input, Reset()).Times(1);

    EXPECT_CALL(input, ReadTo(_))
        .WillOnce(Invoke([](auto* const aBuffer) {
            std::string data = "AAABBC";
            aBuffer->assign(data.begin(), data.end());
            return true;
        }))
        .WillOnce(Return(false))
        .WillOnce(Invoke([](auto* const aBuffer) {
            std::string data = "AAABBC";
            aBuffer->assign(data.begin(), data.end());
            return true;
        }))
        .WillOnce(Return(false));

    Processor::Encode(input, output);
}

