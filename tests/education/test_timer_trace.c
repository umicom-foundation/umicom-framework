/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education/test_timer_trace.c
 * PURPOSE: Exercise monotonic timestamps, finite and indefinite waits,
 *          cancellation, exact-deadline precedence, malformed records,
 *          unchanged reports and literal file paths containing spaces.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/timer_trace.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHECK(test) do { if (!(test)) { \
    fprintf(stderr, "[FAIL] %s:%d: %s\n", __FILE__, __LINE__, #test); \
    return 1; } } while (0)

static void Put16(unsigned char *p, uint16_t v)
{
    p[0] = (unsigned char)v; p[1] = (unsigned char)(v >> 8U);
}
static void Put32(unsigned char *p, uint32_t v)
{
    for (unsigned i = 0U; i < 4U; ++i) p[i] = (unsigned char)(v >> (8U * i));
}
static void Put64(unsigned char *p, uint64_t v)
{
    Put32(p, (uint32_t)v); Put32(p + 4U, (uint32_t)(v >> 32U));
}
static void Header(unsigned char *p, uint32_t count)
{
    const unsigned char magic[8] = {'U','M','I','T','M','R','1',0};
    memcpy(p, magic, sizeof(magic));
    Put16(p + 8U, 1U); Put16(p + 10U, 0U); Put32(p + 12U, count);
}
static size_t Record(unsigned char *p, uint32_t number, unsigned event,
                     uint32_t task, uint32_t key, uint64_t tick, uint64_t value)
{
    Put32(p, number); p[4] = (unsigned char)event; p[5] = 0U;
    Put16(p + 6U, 0U); Put32(p + 8U, task); Put32(p + 12U, key);
    Put64(p + 16U, tick); Put64(p + 24U, value);
    return UMI_EDUCATION_TIMER_RECORD_BYTES;
}
static size_t Standard(unsigned char *buffer)
{
    const struct Step {
        unsigned event; uint32_t task, key; uint64_t tick, value;
    } steps[] = {
        {4U, 0U, 0U, 10U, 0U},
        {1U, 10U, 7U, 10U, 5U},
        {1U, 20U, 8U, 10U, 0U},
        {4U, 0U, 0U, 12U, 0U},
        {2U, 20U, 8U, 12U, 0U},
        {4U, 0U, 0U, 15U, 0U},
        {1U, 30U, 9U, 15U, 3U},
        {3U, 30U, 9U, 15U, 0U},
        {4U, 0U, 0U, 16U, 0U},
        {1U, 40U, 10U, 16U, 1U},
        {4U, 0U, 0U, 17U, 0U}
    };
    const uint32_t count = (uint32_t)(sizeof(steps) / sizeof(steps[0]));
    Header(buffer, count);
    size_t used = UMI_EDUCATION_TIMER_HEADER_BYTES;
    for (uint32_t i = 0U; i < count; ++i)
        used += Record(buffer + used, i + 1U, steps[i].event,
                       steps[i].task, steps[i].key, steps[i].tick, steps[i].value);
    return used;
}
static int Refused(const unsigned char *bytes, size_t length)
{
    UmiEducationTimerReport report;
    memset(&report, 0xa5, sizeof(report));
    const UmiEducationTimerReport saved = report;
    return UmiEducationTimerTraceInspect(bytes, length, &report) != UMI_STATUS_OK &&
           memcmp(&report, &saved, sizeof(report)) == 0;
}
static int CheckStandard(const char *fixture)
{
    unsigned char bytes[UMI_EDUCATION_TIMER_MAX_FILE_BYTES + 1U] = {0};
    const size_t length = Standard(bytes);
    UmiEducationTimerReport report = {0};
    CHECK(length == 16U + 11U * 32U);
    CHECK(UmiEducationTimerTraceInspect(bytes, length, &report) == UMI_STATUS_OK);
    CHECK(report.recordCount == 11U && report.waits == 4U &&
          report.signalled == 1U && report.timedOut == 2U &&
          report.cancelled == 1U && report.advances == 5U &&
          report.peakPending == 2U && report.pendingAtEnd == 0U &&
          report.distinctIdentities == 4U && report.lastTick == 17U &&
          report.nextFiniteDeadline == 0U);
    FILE *file = fopen(fixture, "wb");
    CHECK(file != NULL);
    CHECK(fwrite(bytes, 1U, length, file) == length);
    CHECK(fclose(file) == 0);
    UmiEducationTimerReport readBack = {0};
    CHECK(UmiEducationTimerTraceInspectFile(fixture, &readBack) == UMI_STATUS_OK);
    CHECK(readBack.timedOut == report.timedOut && readBack.lastTick == report.lastTick);
    CHECK(UmiEducationTimerTraceInspectFile("not-an-existing-timer-fixture-404", &readBack) != UMI_STATUS_OK);
    return 0;
}
static int CheckInvalid(void)
{
    unsigned char bytes[UMI_EDUCATION_TIMER_MAX_FILE_BYTES + 1U] = {0};
    const size_t length = Standard(bytes);
    for (size_t cut = 0U; cut < length; ++cut) CHECK(Refused(bytes, cut));
    CHECK(Refused(bytes, length + 1U));
    CHECK(Refused(bytes, UMI_EDUCATION_TIMER_MAX_FILE_BYTES + 1U));
    bytes[0] = 'Z'; CHECK(Refused(bytes, length)); bytes[0] = 'U';
    bytes[8] = 2U; CHECK(Refused(bytes, length)); bytes[8] = 1U;
    bytes[10] = 1U; CHECK(Refused(bytes, length)); bytes[10] = 0U;
    Put32(bytes + 12U, UMI_EDUCATION_TIMER_MAX_RECORDS + 1U);
    CHECK(Refused(bytes, length)); Put32(bytes + 12U, 11U);
    /* Violated strictly increasing tick. */
    Put64(bytes + 16U + 3U * 32U + 16U, 10U);
    CHECK(Refused(bytes, length)); Put64(bytes + 16U + 3U * 32U + 16U, 12U);
    /* An action is not permitted to advance the clock silently. */
    Put64(bytes + 16U + 4U * 32U + 16U, 13U);
    CHECK(Refused(bytes, length)); Put64(bytes + 16U + 4U * 32U + 16U, 12U);
    /* A stale signal must not award a different wait. */
    Put32(bytes + 16U + 4U * 32U + 12U, 9U);
    CHECK(Refused(bytes, length)); Put32(bytes + 16U + 4U * 32U + 12U, 8U);
    /* No cancelled identity may be registered again. */
    Put32(bytes + 16U + 9U * 32U + 8U, 30U);
    CHECK(Refused(bytes, length)); Put32(bytes + 16U + 9U * 32U + 8U, 40U);
    /* Wrong record ordinal, reserved bytes, unknown event. */
    Put32(bytes + 16U + 32U, 10U);
    CHECK(Refused(bytes, length)); Put32(bytes + 16U + 32U, 2U);
    bytes[16U + 5U] = 1U; CHECK(Refused(bytes, length)); bytes[16U + 5U] = 0U;
    Put16(bytes + 16U + 6U, 1U); CHECK(Refused(bytes, length));
    Put16(bytes + 16U + 6U, 0U);
    bytes[16U + 32U + 4U] = 255U;
    CHECK(Refused(bytes, length)); bytes[16U + 32U + 4U] = 1U;
    /* SIGNAL exactly at the finite deadline must be refused, not credited. */
    Header(bytes, 3U);
    Record(bytes + 16U, 1U, 1U, 7U, 77U, 0U, 5U);
    Record(bytes + 48U, 2U, 4U, 0U, 0U, 5U, 0U);
    Record(bytes + 80U, 3U, 2U, 7U, 77U, 5U, 0U);
    CHECK(Refused(bytes, 16U + 3U * 32U));
    /* Deadline addition must refuse wrap without publishing a result. */
    Header(bytes, 2U);
    Record(bytes + 16U, 1U, 4U, 0U, 0U, UINT64_MAX, 0U);
    Record(bytes + 48U, 2U, 1U, 7U, 77U, UINT64_MAX, 1U);
    CHECK(Refused(bytes, 16U + 2U * 32U));
    /* A pending wait with no deadline is still valid at a huge clock value. */
    Record(bytes + 48U, 2U, 1U, 7U, 77U, UINT64_MAX, 0U);
    UmiEducationTimerReport atMax = {0};
    CHECK(UmiEducationTimerTraceInspect(bytes, 16U + 2U * 32U, &atMax) == UMI_STATUS_OK);
    CHECK(atMax.nextFiniteDeadline == 0U && atMax.pendingAtEnd == 1U);
    /* A finite pending wait exposes the earliest deadline for a dispatcher. */
    Header(bytes, 1U);
    Record(bytes + 16U, 1U, 1U, 99U, 9U, 0U, 5U);
    CHECK(UmiEducationTimerTraceInspect(bytes, 48U, &atMax) == UMI_STATUS_OK);
    CHECK(atMax.nextFiniteDeadline == 5U && atMax.pendingAtEnd == 1U);
    /* The inspector must survive single-byte mutations without OOB access. */
    const size_t standardSize = Standard(bytes);
    uint32_t seed = UINT32_C(0x2c4d62ab);
    for (unsigned i = 0U; i < 20000U; ++i) {
        seed = seed * UINT32_C(1664525) + UINT32_C(1013904223);
        const size_t offset = (size_t)(seed % (uint32_t)standardSize);
        const unsigned char saved = bytes[offset];
        bytes[offset] ^= (unsigned char)((seed >> 16U) | 1U);
        UmiEducationTimerReport mutated = {0};
        (void)UmiEducationTimerTraceInspect(bytes, standardSize, &mutated);
        bytes[offset] = saved;
    }
    CHECK(UmiEducationTimerTraceInspect(bytes, standardSize, &atMax) == UMI_STATUS_OK);
    return 0;
}
static int CheckCapacity(void)
{
    unsigned char bytes[UMI_EDUCATION_TIMER_MAX_FILE_BYTES + 1U] = {0};
    Header(bytes, UMI_EDUCATION_TIMER_MAX_WAITS + 1U);
    size_t used = 16U;
    for (uint32_t i = 0U; i <= UMI_EDUCATION_TIMER_MAX_WAITS; ++i)
        used += Record(bytes + used, i + 1U, 1U, i + 1U, 6U, 0U, 0U);
    CHECK(Refused(bytes, used));
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2 || argv[1][0] == '\0') return 2;
    CHECK(CheckStandard(argv[1]) == 0);
    CHECK(CheckInvalid() == 0);
    CHECK(CheckCapacity() == 0);
    puts("[OK] Timer trace: finite/indefinite waits, deadline precedence, overflow and 20000 mutations.");
    return 0;
}
