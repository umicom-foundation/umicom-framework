/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_messaging/test_binding_vector.c
 *
 * PURPOSE:
 *   Check logical fingerprints against independent fixed-byte reference vectors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv) {
  static const char *const cases[] = {"envelope", "operation"};
  int selected = Case(argc, argv, cases, 2U);
  /* Reference bytes were independently assembled from the documented
   * length-prefixed, big-endian fields, then SHA-256 hashed. Keep the JSON
   * vector and expected bytes under review when changing logical identity. */
  const unsigned char expected_envelope[32] = {
      0x3bU, 0x88U, 0xf5U, 0xe7U, 0x71U, 0x68U, 0xeeU, 0x42U, 0xa3U, 0x3aU, 0xc9U,
      0x9aU, 0x9eU, 0xe2U, 0xa3U, 0x4bU, 0x37U, 0xe3U, 0x18U, 0x8bU, 0xc6U, 0xe3U,
      0x3dU, 0x4aU, 0xd9U, 0x1bU, 0xb8U, 0xe1U, 0x22U, 0x6bU, 0xdcU, 0x51U};
  const unsigned char expected_operation[32] = {
      0x41U, 0x96U, 0x8cU, 0x2fU, 0xaeU, 0x00U, 0xe9U, 0xacU, 0x54U, 0xe2U, 0x52U,
      0xd5U, 0x5fU, 0x0cU, 0x3eU, 0x08U, 0xbdU, 0x8eU, 0x39U, 0x57U, 0x5fU, 0x3bU,
      0x32U, 0x45U, 0xa0U, 0xabU, 0x14U, 0xc6U, 0xcdU, 0x6aU, 0xf9U, 0x68U};
  UmiUfmpEnvelope e = Envelope();
  CHECK((unsigned int)e.type == 14U);
  unsigned char actual[32];
  OK(selected == 0 ? UmiUfmpEnvelopeDigest(&e, actual) : UmiUfmpOperationDigest(&e, actual));
  CHECK(memcmp(actual, selected == 0 ? expected_envelope : expected_operation, sizeof actual) == 0);
  return 0;
}
