#pragma once

#include <string>

#include <windows.h>

namespace util
{

inline std::string PathNextToExe(const char *relative)
{
    char buf[MAX_PATH] = {};
    GetModuleFileNameA(nullptr, buf, MAX_PATH);

    std::string path(buf);
    const size_t slash = path.find_last_of("\\/");
    if (slash != std::string::npos)
    {
        path.resize(slash + 1);
    }

    path += relative;
    return path;
}

} // namespace util
