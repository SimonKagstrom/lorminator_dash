#pragma once

#include <string>
#include <vector>
#include <functional>

#include <point.hh>

std::vector<std::string> split_string(const std::string &s, const std::string &delims);

bool string_is_integer(const std::string &str, unsigned base = 0);

int64_t string_to_integer(const std::string &str, unsigned base = 0);

void bresenham(const point &from, const point &to, std::function<bool(const point &where)> callback);
