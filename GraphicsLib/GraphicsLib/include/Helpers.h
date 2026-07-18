#pragma once

#include <cassert>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <memory>
#include <cmath>

# if !defined(USING)
#   define IN_USE &&
#   define NOT_IN_USE &&!
#   define USE_IF(x) &&((x) ? 1 : 0)&&
#   define USING(x) (1 x 1)
#endif

#define ALIGN_AS(n) __declspec(align(n))

#define SAFE_RELEASE(p) if (p) { (p)->Release(); (p) = nullptr; }

using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long long;

using int8 = char;
using int16 = short;
using int32 = int;
using int64 = long long;

template <typename T>
using Vector = std::vector<T>;

template <typename T>
using SPtr = std::shared_ptr<T>;

template <typename T>
using WPtr = std::weak_ptr<T>;

class GenAlloc
{
};

template<class T, class... Args>
SPtr<T> MakeSPtr(Args&&... args)
{
  return std::make_shared<T>(std::forward<Args>(args)...);
}
