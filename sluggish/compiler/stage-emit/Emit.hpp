#pragma once

#include <string>

namespace sluggish {

// Run from the repository root. Writes to sluggish/compiler/intermediate/.
void emit(const std::string& filename, const std::string& text);

} // namespace sluggish
