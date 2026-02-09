#include "lib.hpp"

auto main() -> int
{
  auto const lib = library {};

  return lib.name == "interface" ? 0 : 1;
}
