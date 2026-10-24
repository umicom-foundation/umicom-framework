/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web_forms/test_url.c
 * PURPOSE: Prevent malformed percent decoding and partial output regressions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "fixture.h"
#include "umicom/web/url.h"
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1];
  char out[64] = "unchanged";
  if (strcmp(name, "valid") == 0) {
    CHECK(umi_web_url_decode("a%2Fb+c%25", out, sizeof(out)) == UMI_STATUS_OK &&
          strcmp(out, "a/b c%") == 0);
  } else if (strcmp(name, "malformed") == 0) {
    const char *bad[] = {"%", "%2", "prefix%", "prefix%2", "%GG", "%0G"};
    for (size_t i = 0U; i < sizeof(bad) / sizeof(bad[0]); ++i)
      CHECK(umi_web_url_decode(bad[i], out, sizeof(out)) == UMI_STATUS_PARSE_ERROR &&
            strcmp(out, "unchanged") == 0);
  } else if (strcmp(name, "nul") == 0) {
    CHECK(umi_web_url_decode("prefix%00suffix", out, sizeof(out)) == UMI_STATUS_PARSE_ERROR &&
          strcmp(out, "unchanged") == 0);
  } else if (strcmp(name, "capacity") == 0) {
    CHECK(umi_web_url_decode("abc", out, 3U) == UMI_STATUS_CAPACITY_EXCEEDED &&
          strcmp(out, "unchanged") == 0);
  } else if (strcmp(name, "overlap") == 0) {
    strcpy(out, "a%20b");
    CHECK(umi_web_url_decode(out, out + 1, sizeof(out) - 1U) == UMI_STATUS_OK &&
          strcmp(out + 1, "a b") == 0);
  } else if (strcmp(name, "empty") == 0) {
    CHECK(umi_web_url_decode("", out, 1U) == UMI_STATUS_OK && out[0] == '\0');
  } else if (strcmp(name, "high-byte") == 0) {
    CHECK(umi_web_url_decode("%FF", out, sizeof(out)) == UMI_STATUS_OK &&
          (unsigned char)out[0] == 255U);
  } else
    CHECK(0);
  return 0;
}
