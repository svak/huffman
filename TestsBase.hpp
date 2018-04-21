#pragma once

#include <bitset>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::Eq;
using ::testing::Invoke;
using ::testing::Return;
using ::testing::_;

namespace Helpers
{

    template <typename T>
    std::string bits_of(const T value)
    {
        std::bitset<sizeof(value) * 8> v{static_cast<unsigned long long>(value)};
        return v.to_string();
    }
}
