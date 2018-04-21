#pragma once

#include <cstdint>
#include <queue>

#include "Model.hpp"

namespace Helpers
{

    class BitsAdapter
    {
    public:
        BitsAdapter(char* aBuffer, size_t aSize);
        BitsAdapter(BitsAdapter&&);
        BitsAdapter(const BitsAdapter&) = delete;
        BitsAdapter& operator=(const BitsAdapter&) = delete;

        void Write(const unsigned aBits, const unsigned aLength);
        void Reset();

        bool IsFull() const;
        size_t BytesTaken() const;
        size_t RemainingBits() const;

    private:
        void WriteByte(const uint8_t aValue, const unsigned aLength);

    private:
        char* mBegin{nullptr};

        struct
        {
            char* Byte{nullptr};
            unsigned RemainingBits{8};
        } mCursor;

        size_t mTotalBits{0};
        size_t mConsumedBits{0};

        std::queue<std::pair<uint8_t, int>> mTail;
    };

    struct CodeProducer
    {
        static constexpr size_t ValueSize{sizeof(uint8_t) * 8};

        unsigned Value{0};
        unsigned Length{0};

        void Add(const uint8_t aValue);
        bool Next();
        void Rewind();

    private:
        unsigned mData{0};
        unsigned mRemainingBits{sizeof(mData) * 8};

        static_assert(sizeof(unsigned int) == 4, "Platform has wierd 'unsigned int' size.");
    };
}
