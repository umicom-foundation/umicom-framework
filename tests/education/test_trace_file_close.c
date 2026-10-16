/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education/test_trace_file_close.c
 * PURPOSE: Keep trace reports unchanged when input close reports an I/O error.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/education/schedule_trace.h"
#include "umicom/education/timer_trace.h"
static int failClose;
static unsigned closeCount;
/* Close the real test file exactly once, then model the late I/O error that
 * ordinary valid-input fixtures cannot reliably produce on every platform. */
static int CloseInput(FILE *file)
{
    ++closeCount;
    int result = fclose(file);
    return failClose ? EOF : result;
}
#define fclose CloseInput
#if defined(UMICOM_TRACE_TIMER)
#include "../../src/education/timer_trace.c"
typedef UmiEducationTimerReport Report;
#define INSPECT UmiEducationTimerTraceInspectFile
static const unsigned char validTrace[16] = {'U','M','I','T','M','R','1',0,1,0,0,0,0,0,0,0};
#elif defined(UMICOM_TRACE_INTERRUPT)
#include "../../src/education/interrupt_trace.c"
typedef UmiEducationInterruptReport Report;
#define INSPECT UmiEducationInterruptTraceInspectFile
static const unsigned char validTrace[16] = {'U','M','I','I','R','Q','1',0,1};
#elif defined(UMICOM_TRACE_EVIDENCE)
#include "../../src/education/evidence_trace.c"
typedef UmiEducationEvidenceReport Report;
#define INSPECT UmiEducationEvidenceTraceInspectFile
static const unsigned char validTrace[24] = {'U','M','I','E','V','D','1',0,1};
#elif defined(UMICOM_TRACE_DEVICE_IO)
#include "../../src/education/device_io_trace.c"
typedef UmiEducationDeviceIoReport Report;
#define INSPECT UmiEducationDeviceIoTraceInspectFile
static const unsigned char validTrace[24] = {'U','M','I','D','I','O','1',0,1};
#else
#include "../../src/education/schedule_trace.c"
typedef UmiEducationScheduleReport Report;
#define INSPECT UmiEducationScheduleTraceInspectFile
/* The original initializer supplied seventeen bytes to a sixteen-byte header.
 * Retain it for review; explicit header fields below leave the record count
 * and reserved bytes zero so this fixture exercises close errors on valid input. */
#if 0
static const unsigned char validTrace[16] = {'U','M','I','S','C','H','1',0,1,0,0,0,0,0,0,0,0};
#endif
static const unsigned char validTrace[16] = {
    'U', 'M', 'I', 'S', 'C', 'H', '1', 0, 1
};
#endif
#undef fclose
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "Line %d: %s\n", __LINE__, #condition); return 1; \
} } while (0)
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    FILE *file = fopen(argv[1], "wb");
    CHECK(file != NULL);
    size_t written = fwrite(validTrace, 1U, sizeof(validTrace), file);
    int closed = fclose(file);
    CHECK(written == sizeof(validTrace) && closed == 0);
    Report before, output;
    memset(&before, 0x5a, sizeof(before));
    memcpy(&output, &before, sizeof(output));
    failClose = 1;
    CHECK(INSPECT(argv[1], &output) == UMI_STATUS_IO_ERROR);
    CHECK(closeCount == 1U && memcmp(&before, &output, sizeof(output)) == 0);
    failClose = 0; closeCount = 0U;
    CHECK(INSPECT(argv[1], &output) == UMI_STATUS_OK);
    CHECK(output.recordCount == 0U && closeCount == 1U);
    CHECK(remove(argv[1]) == 0);
    return 0;
}
