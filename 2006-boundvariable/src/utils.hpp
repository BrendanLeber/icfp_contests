#pragma once

#include <cstdint>
#include <vector>

using Platter = uint32_t;

using Scroll = std::vector<Platter>;

Scroll load(const std::string& fname);
