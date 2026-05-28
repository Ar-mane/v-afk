#pragma once

#include <array>
#include <string_view>

inline constexpr int kMsPerChar = 500;

inline constexpr std::array<std::string_view, 10> kScenarios = {{
    "aaaaaaaaaaaddddddddddcc",
    "adadadadadadadadadad",
    "sssssaaaaaaadddddcc",
    "adadadadadadadadccssss",
    "aaaaaaaaadddcc",
    "ssssadadadadadsssscc",
    "ccadadadadadadcc",
    "aaaaaaaaaaaaaddddddddddcc",
    "ssadadadadadsscc",
    "ddddddaaaaaasssscc",
}};
