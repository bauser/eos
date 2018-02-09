#include "cameleon.hpp"

extern "C" {
  void apply(uint64_t code, uint64_t action) {
    cameleon::contract<N(cameleon)>::apply(code, action);
  }
}
