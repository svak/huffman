#pragma once

#include <fstream>

#include "Interfaces.hpp"

namespace Filesystem
{
    struct Reader final : IStorage::Input
    {
        Reader(const std::string& aName);
        ~Reader();

        Reader(const Reader&) = delete;
        Reader& operator=(const Reader&) = delete;
        Reader(Reader&&) = delete;

        virtual bool ReadTo(TBuffer* aBuffer) override;
        virtual size_t Read(char* aBuffer, size_t aSize) override;
        virtual void Reset() override;

    private:
        std::ifstream mStream;
    };

    struct Writer final : IStorage::Output
    {
        Writer(const std::string& aName);
        ~Writer();

        Writer(const Writer&) = delete;
        Writer& operator=(const Writer&) = delete;
        Writer(Writer&&) = delete;

        virtual void Write(const char* aBuffer, size_t aSize) override;
        virtual void Skip(const size_t aSize) override;
        virtual void Reset() override;

    private:
        std::ofstream mStream;
    };
}
