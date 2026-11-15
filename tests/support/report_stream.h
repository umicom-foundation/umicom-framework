/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/support/report_stream.h
 *
 * PURPOSE:
 *   Inject deterministic standard-output write and buffered-delivery failures without changing process streams.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_REPORT_STREAM_H
#define UMICOM_TEST_REPORT_STREAM_H
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REPORT_CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); exit(1); \
} } while (0)

/* Only the included command's stdio calls are redirected by the caller's macros.
 * SDK services still read real files; no OS handle is closed or replaced. */
static char ReportTestText[262144U];
static size_t ReportTestLength, ReportTestWrites, ReportTestFailWrite;
static unsigned ReportTestFlushes, ReportTestErrors;
static int ReportTestFlushFailure, ReportTestStreamFailure;

static void ReportTestReset(const char *mode, size_t last_write)
{
    ReportTestText[0] = '\0';
    ReportTestLength = ReportTestWrites = ReportTestFailWrite = 0U;
    ReportTestFlushes = ReportTestErrors = 0U;
    ReportTestFlushFailure = ReportTestStreamFailure = 0;
    if (strcmp(mode, "write-first") == 0) ReportTestFailWrite = 1U;
    else if (strcmp(mode, "write-last") == 0) ReportTestFailWrite = last_write;
    else if (strcmp(mode, "flush") == 0) ReportTestFlushFailure = 1;
    else if (strcmp(mode, "stream") == 0) ReportTestStreamFailure = 1;
    else if (strcmp(mode, "flush-and-stream") == 0) {
        ReportTestFlushFailure = ReportTestStreamFailure = 1;
    } else REPORT_CHECK(strcmp(mode, "success") == 0);
}
static int ReportTestPrintf(const char *format, ...)
{
    ++ReportTestWrites;
    if (ReportTestWrites == ReportTestFailWrite) return -1;
    va_list arguments;
    va_start(arguments, format);
    int count = vsnprintf(ReportTestText + ReportTestLength,
        sizeof ReportTestText - ReportTestLength, format, arguments);
    va_end(arguments);
    REPORT_CHECK(count >= 0 && (size_t)count < sizeof ReportTestText - ReportTestLength);
    ReportTestLength += (size_t)count;
    return count;
}
static int ReportTestPuts(const char *text)
{
    return ReportTestPrintf("%s\n", text) < 0 ? EOF : 0;
}
static int ReportTestFlush(FILE *stream)
{
    REPORT_CHECK(stream == stdout);
    ++ReportTestFlushes;
    return ReportTestFlushFailure ? EOF : 0;
}
static int ReportTestError(FILE *stream)
{
    REPORT_CHECK(stream == stdout);
    ++ReportTestErrors;
    return ReportTestStreamFailure;
}
#endif
