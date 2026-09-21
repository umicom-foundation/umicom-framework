/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_execution/test_process_stream.c
 * PURPOSE: Verify raw output delivery independently of the retained diagnostic tail.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "json_test_support.h"
#include "umicom/platform/process.h"
typedef struct Stream { size_t length; size_t calls; uint64_t hash; } Stream;
static void Observe(const char *bytes, size_t length, void *context)
{
    Stream *stream = (Stream *)context;
    REQUIRE(bytes != NULL && length != 0U);
    stream->length += length; ++stream->calls;
    for (size_t i = 0U; i < length; ++i) { stream->hash ^= (unsigned char)bytes[i]; stream->hash *= UINT64_C(1099511628211); }
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    UmiProcessRequest request = {0}; UmiProcessResult result;
    const char *arguments[] = {"large"};
    Stream actual = {0U, 0U, UINT64_C(14695981039346656037)}, expected = actual;
    request.program = argv[1]; request.arguments = arguments; request.argument_count = 1U;
    request.capture_stdout = 1; request.capture_stderr = 1; request.timeout_ms = 30000U;
    REQUIRE(UmiProcessExecuteStreamed(&request, Observe, &actual, &result) == UMI_STATUS_INTERNAL_ERROR);
    const char *line = "Umicom test output: deterministic text to exercise bounded capture and result-file reading."
#ifdef _WIN32
        "\r\n";
    const char *tail = "UMICOM_TAIL_MARKER\r\n";
#else
        "\n";
    const char *tail = "UMICOM_TAIL_MARKER\n";
#endif
    for (size_t i = 0U; i < 12000U; ++i) Observe(line, strlen(line), &expected);
    Observe(tail, strlen(tail), &expected);
    REQUIRE(actual.length == expected.length && actual.hash == expected.hash && actual.calls > 1U);
    REQUIRE(result.output_truncated && result.exit_code == 5 && strstr(result.output, "UMICOM_TAIL_MARKER") != NULL);
    printf("Stream bytes=%zu; exact hash matches; retained tail truncated=%d.\n", actual.length, result.output_truncated);
    return 0;
}
