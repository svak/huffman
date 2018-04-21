#include <algorithm>
#include <cassert>
#include <stdexcept>

#include "BitsAdapter.hpp"

namespace Helpers
{
    namespace Utils
    {
        template <typename T>
        auto RotateLeft(const T x, const unsigned int n)
        {
            static_assert(std::is_unsigned<T>::value, "Type must be unsigned");

            auto a = x << n;
            auto b = x >> (sizeof(T) * 8 - n);
            return static_cast<T>(a | b);
        }

        template <typename T>
        auto RotateRight(const T x, const unsigned int n)
        {
            static_assert(std::is_unsigned<T>::value, "Type must be unsigned");

            auto a = x >> n;
            auto b = x << (sizeof(T) * 8 - n);
            return static_cast<T>(a | b);
        }
    }

    BitsAdapter::BitsAdapter(char* aBuffer, size_t aSize)
      : mBegin{aBuffer}
      , mCursor{aBuffer, 8}
      , mTotalBits{aSize * 8}
      , mConsumedBits{0}
    {
        assert(aSize >= sizeof(int) && "BitsAdapter buffer size should be enough for symbol code.");

        Reset();
    }

    void BitsAdapter::Write(const unsigned aCode, const unsigned aLength)
    {
        if (RemainingBits() < aLength && !mTail.empty())
        {
            throw std::runtime_error("BitsAdapter: Not enough space left.");
        }

        auto remaining = aLength;
        auto code = aCode << (32 - aLength);

        while (remaining > 0)
        {
            uint8_t value = (0xFF000000 & code) >> 24;

            auto consume = std::min(8u, remaining);
            WriteByte(value, consume);

            remaining -= consume;
            code <<= 8;
        }
    }

    void BitsAdapter::Reset()
    {
        mCursor.Byte = mBegin;
        *mCursor.Byte = 0x00;
        mCursor.RemainingBits = 8;
        mConsumedBits = 0;

        while (!mTail.empty())
        {
            auto tail = mTail.front();
            WriteByte(tail.first, tail.second);
            mTail.pop();
        }
    }

    bool BitsAdapter::IsFull() const
    {
        return RemainingBits() == 0;
    }

    size_t BitsAdapter::BytesTaken() const
    {
        return (mConsumedBits + 7) / 8;
    }

    size_t BitsAdapter::RemainingBits() const
    {
        return mTotalBits - mConsumedBits;
    }

    void BitsAdapter::WriteByte(const uint8_t aValue, const unsigned aLength)
    {
        auto& cursor = mCursor;

        if (cursor.RemainingBits == 0)
        {
            if (RemainingBits() < aLength)
            {
                mTail.push(std::make_pair(aValue, aLength));
                return;
            }

            cursor.RemainingBits = 8;
            cursor.Byte++;

            *cursor.Byte = 0x00;
        }

        if (cursor.RemainingBits >= aLength)
        {
            auto value = aValue;

            uint8_t mask = ~Utils::RotateLeft(static_cast<uint8_t>(0xFF << aLength), cursor.RemainingBits - aLength);

            auto shift = 8 - cursor.RemainingBits;
            value = Utils::RotateRight(value, shift) & mask;

            *cursor.Byte |= value;

            cursor.RemainingBits -= aLength;
            mConsumedBits += aLength;
        }
        else
        {
            auto remaining = cursor.RemainingBits;
            WriteByte(aValue, remaining);
            WriteByte(Utils::RotateLeft(aValue, remaining), aLength - remaining);
        }
    }

    void CodeProducer::Add(const uint8_t aValue)
    {
        if (mRemainingBits < ValueSize)
        {
            throw std::runtime_error("Producer cache is full.");
        }

        auto shift = mRemainingBits - ValueSize;

        unsigned int value = aValue;
        value <<= shift;

        unsigned int mask = (0xFF << shift);

        mData &= ~mask;
        mData |= value;

        mRemainingBits -= ValueSize;
    }

    bool CodeProducer::Next()
    {
        if (Length == sizeof(mData) * ValueSize - mRemainingBits)
        {
            return false;
        }

        Length++;

        unsigned shift = sizeof(mData) * ValueSize - Length;
        unsigned mask = 0xFFFFFFFF << shift;

        Value = Utils::RotateRight(mData & mask, shift);

        return true;
    }

    void CodeProducer::Rewind()
    {
        mData <<= Length;
        mRemainingBits += Length;
        Length = 0;
    }
}
