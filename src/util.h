#pragma once
#define USE_FMT 1

#include <iostream>
#include <string>

#if USE_FMT
#include <fmt/format.h>
#endif

template<class ...Args>
inline void print_log(const std::string& format, Args ...args) noexcept
{
#if USE_FMT
    auto arg_store = fmt::make_format_args(args...);
    std::string line = fmt::vformat(format, fmt::basic_format_args(arg_store));
    std::cerr << line;
#else
    std::cerr << format;
    ((std::cerr << ' ' << args),...);
#endif
}

bool isNumeric(const std::string& s);
void replaceAll(std::string& str, const std::string& from, const std::string& to);
std::string toLowerCase(const std::string& s);
std::string trimSpaces(const std::string& str);
