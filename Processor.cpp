#include <cassert>
#include <memory>
#include <string>

#include "BitsAdapter.hpp"
#include "HuffmanTree.hpp"
#include "Processor.hpp"
#include "SymbolsLookup.hpp"
#include "StreamWriter.hpp"

namespace Processor
{
    using Model::FileHeader;

    static Config DefaultConfig;

    void CheckConfig(const Config& aConfig)
    {
        static constexpr auto MinReaderSize = sizeof(FileHeader) + sizeof(FileHeader::SymbolsTable::Entry) * 256;

        if (aConfig.Buffer.InputSize < MinReaderSize)
        {
            throw std::runtime_error("Reader buffer is too small. Should be >= " + std::to_string(MinReaderSize));
        };

        if (aConfig.Buffer.OutputSize < sizeof(decltype(Model::SymbolCode::Value)))
        {
            throw std::runtime_error("Writer buffer is too small.");
        };
    }

    void Encode(IStorage::Input& aInput, IStorage::Output& aOutput)
    {
        return Encode(aInput, aOutput, DefaultConfig);
    }

    void Encode(IStorage::Input& aInput, IStorage::Output& aOutput, const Config& aConfig)
    {
        CheckConfig(aConfig);

        std::vector<unsigned char> buffer;
        buffer.reserve(aConfig.Buffer.InputSize);

        // Collect information about source file.
        // Then build Huffman tree.
        size_t fileSize = 0;
        Huffman::TreeBuilder builder;
        while (aInput.ReadTo(&buffer))
        {
            fileSize += buffer.size();
            builder.Process(buffer);
        }
        auto tree = builder.Build();

        // Leave space for header.
        aOutput.Skip(sizeof(FileHeader));

        // Write Huffman code for each symbol.
        for (const auto& p : tree.Codes)
        {
            const auto& symbol = p.first;
            const auto& code = p.second;
            const auto item = FileHeader::SymbolsTable::Entry{symbol, code};
            aOutput.Write(reinterpret_cast<const char*>(&item), sizeof(item));
        }

        // One more time scan source,
        // and encode each symbol with corresponded Huffman code.
        aInput.Reset();

        std::unique_ptr<char> encodedStream{new char[aConfig.Buffer.OutputSize]};
        Helpers::BitsAdapter bits(encodedStream.get(), aConfig.Buffer.OutputSize);

        while (aInput.ReadTo(&buffer))
        {
            for (const auto& c : buffer)
            {
                const auto& item = tree.Codes[c];
                bits.Write(item.Value, item.Length);

                if (bits.IsFull())
                {
                    aOutput.Write(encodedStream.get(), bits.BytesTaken());
                    bits.Reset();
                }
            }
        }

        if (bits.BytesTaken())
        {
            aOutput.Write(encodedStream.get(), bits.BytesTaken());
            bits.Reset();
        }

        // Finally, we stamp file header and we done.
        aOutput.Reset();

        FileHeader header;
        header.FileSize = fileSize;
        header.Table.Length = tree.Codes.size();
        aOutput.Write(reinterpret_cast<char*>(&header), sizeof(FileHeader));
    }

    void Decode(IStorage::Input& aInput, IStorage::Output& aOutput)
    {
        return Decode(aInput, aOutput, DefaultConfig);
    }

    void Decode(IStorage::Input& aInput, IStorage::Output& aOutput, const Config& aConfig)
    {
        CheckConfig(aConfig);

        // We must be sure that given file
        // has correct header and symbols table.
        std::unique_ptr<char> source{new char[aConfig.Buffer.InputSize]};

        auto count = aInput.Read(source.get(), aConfig.Buffer.InputSize);
        if (count < sizeof(FileHeader))
        {
            throw std::runtime_error("Decoder: File has no header.");
        }

        auto header = reinterpret_cast<FileHeader*>(source.get());
        auto fileSize = header->FileSize;
        auto symbolsTableSize = sizeof(FileHeader) + header->Table.Length * sizeof(FileHeader::SymbolsTable::Entry);

        if (count < symbolsTableSize)
        {
            throw std::runtime_error("Decoder: Invalid symbols table.");
        }

        // Build Huffman codes lookup table.
        Helpers::SymbolsLookup lookup;
        for (size_t i = 0; i < header->Table.Length; ++i)
        {
            lookup.Put(header->Table.Entries[i]);
        }

        // Get compressed data offset.
        char* data = source.get() + symbolsTableSize;
        count -= symbolsTableSize;

        // Now, scan compressed data and for each given set of bits we
        // look for corresponded symbol. If we have one, then write it down.
        Helpers::CodeProducer producer;
        Helpers::StreamWriter<> stream(aOutput);

        size_t totalSize = 0;

        auto decoder = [&producer, &lookup, &stream, &totalSize, fileSize](const char* aData, size_t aSize) {
            for (size_t i = 0; i < aSize; ++i)
            {
                const auto& value = aData[i];
                producer.Add(value);

                // Enumerate all possible code from current 'point-of-view'.
                // And try to find corresponded symbol.
                while (producer.Next())
                {
                    auto find = lookup.Find(producer.Value, producer.Length);
                    if (find.Success)
                    {
                        stream.Write(find.Symbol);
                        producer.Rewind();

                        totalSize += 1;
                        if (totalSize == fileSize)
                        {
                            return false;
                        }
                    }
                }
            }
            return true;
        };

        while (count > 0)
        {
            if (!decoder(data, count))
            {
                break;
            }

            count = aInput.Read(source.get(), aConfig.Buffer.InputSize);
            data = source.get();
        }

        stream.Flush();
    }
}
