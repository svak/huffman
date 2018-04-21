#include "Filesystem.hpp"

namespace Filesystem
{
    static constexpr size_t BufferSize = 4096;

    Reader::Reader(const std::string& aName)
      : mStream{aName, std::ios::in | std::ios::binary}
    {
    }

    Reader::~Reader()
    {
        mStream.close();
    }

    bool Reader::ReadTo(TBuffer* aBuffer)
    {
        char buffer[BufferSize];

        if (!mStream.good())
        {
            return false;
        }

        mStream.read(buffer, BufferSize);

        auto count = mStream.gcount();
        aBuffer->assign(buffer, buffer + count);

        return count != 0;
    }

    size_t Reader::Read(char* aBuffer, size_t aSize)
    {
        mStream.read(aBuffer, aSize);
        return mStream.gcount();
    }

    void Reader::Reset()
    {
        mStream.clear();
        mStream.seekg(0, std::ios_base::beg);
    }

    Writer::Writer(const std::string& aName)
      : mStream{aName, std::ios::out | std::ios::binary}
    {
    }

    Writer::~Writer()
    {
        mStream.close();
    }

    void Writer::Write(const char* aBuffer, size_t aSize)
    {
        mStream.write(aBuffer, aSize);

        if (!mStream.good())
        {
            throw std::runtime_error("File writing error");
        }
    }

    void Writer::Skip(const size_t aSize)
    {
        mStream.seekp(aSize, std::ios_base::cur);
    }

    void Writer::Reset()
    {
        mStream.clear();
        mStream.seekp(0, std::ios_base::beg);
    }
}
