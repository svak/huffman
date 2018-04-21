#pragma once

#include <cstddef>
#include <vector>

struct InterfaceBase
{
    virtual ~InterfaceBase() = default;
};

struct IStorage
{

    struct Input : InterfaceBase
    {
        using TBuffer = std::vector<unsigned char>;

        virtual bool ReadTo(TBuffer* const) = 0;
        virtual size_t Read(char*, const size_t) = 0;
        virtual void Reset() = 0;
    };

    struct Output : InterfaceBase
    {
        virtual void Write(const char* aBuffer, const size_t aSize) = 0;
        virtual void Skip(const size_t aSize) = 0;
        virtual void Reset() = 0;
    };
};
