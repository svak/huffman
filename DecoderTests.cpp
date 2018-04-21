#include "Interfaces.hpp"
#include "Processor.hpp"
#include "TestsBase.hpp"

using Helpers::bits_of;
using Model::FileHeader;

struct InputFileMock : IStorage::Input
{
    MOCK_METHOD1(ReadTo, bool(IStorage::Input::TBuffer* const));
    MOCK_METHOD2(Read, size_t(char*, const size_t));
    MOCK_METHOD0(Reset, void());
};

struct OutputFileMock : IStorage::Output
{
    MOCK_METHOD2(Write, void(const char*, const size_t));
    MOCK_METHOD1(Skip, void(const size_t));
    MOCK_METHOD0(Reset, void());
};

struct EncodedFile
{
    EncodedFile& WithFileSize(int aSize)
    {
        mFileSize = aSize;
        return *this;
    }
    EncodedFile& WithTableLength(int aLength)
    {
        mLength = aLength;
        return *this;
    }

    EncodedFile& WithEntry(uint8_t aSymbol, int aCode, int aLength)
    {
        mEntries.emplace_back(FileHeader::SymbolsTable::Entry{aSymbol, {aCode, aLength}});
        return *this;
    }

    EncodedFile& WithData(uint8_t aData)
    {
        mData.emplace_back(aData);
        return *this;
    }

    auto Build() const
    {
        std::vector<char> file;

        size_t headerSize = 0;
        headerSize += sizeof(FileHeader);
        headerSize += sizeof(FileHeader::SymbolsTable::Entry) * mEntries.size();

        size_t totalSize = 0;
        totalSize += headerSize;
        totalSize += mData.size();

        file.resize(totalSize);

        // Write header and symbols table.
        auto header = reinterpret_cast<FileHeader*>(file.data());
        header->FileSize = mFileSize;
        header->Table.Length = mLength;

        for (size_t i = 0; i < mEntries.size(); ++i)
        {
            header->Table.Entries[i] = mEntries[i];
        }

        // Write data.
        std::copy(mData.begin(), mData.end(), file.begin() + headerSize);

        return file;
    }

private:
    int mLength{0};
    int mFileSize{0};
    std::vector<FileHeader::SymbolsTable::Entry> mEntries;
    std::vector<char> mData;
};

void SetupSourceFile(InputFileMock& aInput, const std::vector<char>& aFile)
{
    EXPECT_CALL(aInput, Read(_, _))
      .WillOnce(Invoke([&aFile](char* aBuffer, const size_t aSize) {
          EXPECT_GE(aSize, aFile.size());
          std::copy(aFile.begin(), aFile.end(), aBuffer);
          return aFile.size();
      }))
      .WillRepeatedly(Return(0));
}

void ExpectWrite(OutputFileMock& aOutput, const std::string& aData)
{
    EXPECT_CALL(aOutput, Write(_, _)).WillOnce(Invoke([aData](const char* aBuffer, const size_t aSize) {
        ASSERT_EQ(aData.size(), aSize);
        std::string actual;
        actual.assign(aBuffer, aBuffer + aSize);
        EXPECT_EQ(aData, actual);
    }));
}

TEST(Decoder, ShouldThrowIfNoFileHeaderFound)
{
    // Given source file with no header at all.
    InputFileMock input;
    OutputFileMock output;

    EXPECT_CALL(input, Read(_, _)).WillOnce(Return(sizeof(FileHeader) - 1));

    // We expect that exception will be thrown.
    // When we process that file.
    EXPECT_ANY_THROW(Processor::Decode(input, output));
}

TEST(Decoder, ShouldThrowWhenSymbolsTableIsInvalid)
{
    // Given source file with missing symbols entries.
    InputFileMock input;
    OutputFileMock output;

    auto file = EncodedFile().WithTableLength(10).WithEntry('A', 0x00, 2).Build();
    SetupSourceFile(input, file);

    // We expect that exception will be thrown..
    // When we process that file.
    EXPECT_ANY_THROW(Processor::Decode(input, output));
}

TEST(Decoder, ShouldAcceptValidSymbolsTable)
{
    // Given source file with correct symbols table.
    InputFileMock input;
    OutputFileMock output;

    auto file = EncodedFile().WithTableLength(2).WithEntry('A', 0x00, 2).WithEntry('B', 0x00, 2).Build();

    SetupSourceFile(input, file);

    // We expect that decoder will accept that.
    // When we process that file.
    Processor::Decode(input, output);
}

TEST(Decoder, ShouldDecodeSequenceWithRespectToFileSize)
{
    // Given encoded file.
    InputFileMock input;
    OutputFileMock output;

    auto file = EncodedFile().WithFileSize(3).WithTableLength(1).WithEntry('A', 0b1, 1).WithData(0b11111111).Build();

    SetupSourceFile(input, file);

    // We expect that file will be decoded.
    ExpectWrite(output, "AAA");

    // When we process that file.
    Processor::Decode(input, output);
}

TEST(Decoder, ShouldDecodeSequenceWithMinimumBufferSize)
{
    // Given encoded file.
    InputFileMock input;
    OutputFileMock output;

    auto file = EncodedFile()
                  .WithFileSize(63)
                  .WithTableLength(2)
                  .WithEntry('A', 0b1, 1)
                  .WithEntry('B', 0b01, 2)
                  .WithData(0b11111111) // AAAAAAAA
                  .WithData(0b11111111) // AAAAAAAA
                  .WithData(0b11111111) // AAAAAAAA
                  .WithData(0b11111111) // AAAAAAAA
                  .WithData(0b11111111) // AAAAAAAA
                  .WithData(0b11111111) // AAAAAAAA
                  .WithData(0b11111111) // AAAAAAAA
                  .WithData(0b10101010) // ABBB..
                  .WithData(0b10110000) // BBA
                  .Build();

    SetupSourceFile(input, file);

    // We expect that file will be decoded.
    ExpectWrite(output,
                "AAAAAAAA"
                "AAAAAAAA"
                "AAAAAAAA"
                "AAAAAAAA"
                "AAAAAAAA"
                "AAAAAAAA"
                "AAAAAAAA"
                "ABBB"
                "BBA");

    // When we process file with small buffer size.
    Processor::Config config;
    config.Buffer.OutputSize = 4;

    Processor::Decode(input, output, config);
}

TEST(Decoder, ShouldDecodeSequenceWithLongCodedSymbol)
{
    // Given encoded file.
    InputFileMock input;
    OutputFileMock output;

    auto file = EncodedFile()
                  .WithFileSize(5)
                  .WithTableLength(2)
                  .WithEntry('A', 0b11111111111111111111111111111111, 32)
                  .WithEntry('B', 0b01, 2)
                  .WithData(0b11111111) // A..
                  .WithData(0b11111111) //
                  .WithData(0b11111111) //
                  .WithData(0b11111111) //
                  .WithData(0b01010101) // BBBB
                  .Build();

    SetupSourceFile(input, file);

    // We expect that file will be decoded.
    ExpectWrite(output, "ABBBB");

    // When we process file with small buffer size.
    Processor::Config config;
    config.Buffer.OutputSize = 4;

    Processor::Decode(input, output, config);
}
