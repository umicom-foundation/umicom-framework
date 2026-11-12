/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/video_sequence/tool_fixture.c
 *
 * PURPOSE:
 *   Emulate encoder pipe outcomes and verify captured SVG lifetime without claiming playable video.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/clock.h"
#include "umicom/platform/input_file.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif
int main(int argc, char **argv) {
#ifdef _WIN32
  (void)_setmode(_fileno(stdout), _O_BINARY);
#endif
  const char *input = "";
  for (int i = 1; i + 1 < argc; ++i)
    if (strcmp(argv[i], "-i") == 0)
      input = argv[i + 1];
  /* Native export dispatch must keep its private SVG alive until the child
   * has consumed it. Byte-for-byte canonical identity has separate resource
   * coverage; this fixture checks lifetime and decoder selection only. */
  bool branded = false, svgDecoder = false;
  for (int i = 1; i + 1 < argc; ++i) {
    if (strcmp(argv[i], "-filter_complex") == 0 && strstr(argv[i + 1], "[umi_branded]"))
      branded = true;
    if (strcmp(argv[i], "-c:v") == 0 && strcmp(argv[i + 1], "librsvg") == 0)
      svgDecoder = true;
  }
  if (branded) {
    unsigned char *bytes = NULL;
    size_t size = 0U;
    if (!svgDecoder || UmiInputFileRead(input, 131072U, &bytes, &size) != UMI_STATUS_OK)
      return 41;
    bool svg = false;
    for (size_t i = 0U; i + 4U <= size; ++i)
      if (memcmp(bytes + i, "<svg", 4U) == 0) { svg = true; break; }
    UmiInputFileFree(bytes);
    if (!svg) return 42;
  }
  if (strstr(input, "timeout") != NULL) {
    UmiClock clock = umi_clock_system();
    (void)clock.sleep_milliseconds(&clock, 10000U);
    umi_clock_dispose(&clock);
  }
  if (strstr(input, "empty") != NULL)
    return 0;
  if (strstr(input, "diagnostics") != NULL) {
    for (unsigned i = 0U; i < 800U; ++i)
      fputs("encoder diagnostic\n", stderr);
    fputs("diagnostic-tail-marker\n", stderr);
    (void)fflush(stderr);
  }
  /* This prefix is only a transport fixture. The separate actual-encoder
   * acceptance case decodes real media and checks frame count and colour. */
  static const unsigned char prefix[] = {0,   0,   0,   16,  'f', 't', 'y', 'p',
                                         'i', 's', 'o', 'm', 0,   0,   0,   0};
  if (fwrite(prefix, 1U, sizeof(prefix), stdout) != sizeof(prefix))
    return 3;
  (void)fflush(stdout);
  if (strstr(input, "late-cancel") != NULL || strstr(input, "closed-output") != NULL) {
    /* Output closure is not process success. This lets the owner qualification
     * distinguish a live process after EOF from a fully completed encoder. */
    bool closed = strstr(input, "closed-output") != NULL;
    if (closed)
      (void)fclose(stdout);
    UmiClock clock = umi_clock_system();
    (void)clock.sleep_milliseconds(&clock, closed ? 250U : 10000U);
    umi_clock_dispose(&clock);
    if (closed)
      return 7;
  }
  return strstr(input, "failed") != NULL ? 9 : 0;
}
