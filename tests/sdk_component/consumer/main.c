/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#include <sdk_fixture.h>

int main(void) {
  /* Explicit checks remain effective with Release assertions disabled. */
  if (sdk_composed_value() != 42) {
    return 1;
  }
#ifdef SDK_HAS_DEFERRED
  /* Deferred registration must contribute a usable installed archive. */
  if (sdk_deferred_value() != 7) {
    return 2;
  }
#endif
  return 0;
}
