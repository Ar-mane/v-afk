#pragma once

#include "imgui.h"

namespace util
{

constexpr int HexDigit(char c)
{
    return (c >= '0' && c <= '9')   ? c - '0'
           : (c >= 'a' && c <= 'f') ? c - 'a' + 10
           : (c >= 'A' && c <= 'F') ? c - 'A' + 10
                                    : 0;
}

constexpr unsigned int ParseHexRgb(const char *hex)
{
    const char *p = (hex[0] == '#') ? hex + 1 : hex;
    return (HexDigit(p[0]) << 20) | (HexDigit(p[1]) << 16) | (HexDigit(p[2]) << 12) |
           (HexDigit(p[3]) << 8) | (HexDigit(p[4]) << 4) | HexDigit(p[5]);
}

constexpr ImU32 Hex(const char *hex, int alpha = 255)
{
    const unsigned int rgb = ParseHexRgb(hex);
    return IM_COL32((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF, alpha);
}

constexpr ImVec4 Hex4(const char *hex, float alpha = 1.f)
{
    const unsigned int rgb = ParseHexRgb(hex);
    return ImVec4(((rgb >> 16) & 0xFF) / 255.f, ((rgb >> 8) & 0xFF) / 255.f, (rgb & 0xFF) / 255.f,
                  alpha);
}

} // namespace util
