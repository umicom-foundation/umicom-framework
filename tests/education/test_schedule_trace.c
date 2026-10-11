/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education/test_schedule_trace.c
 * PURPOSE: Verify independent trace state, fair ready order, waiting ownership,
 *          cancellation, hostile bytes, unchanged reports and spaced file paths.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/schedule_trace.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "[FAIL] %s:%d: %s\n", __FILE__, __LINE__, #condition); \
    return 1; } } while (0)

static void Put16(unsigned char *b, uint16_t v)
{
    b[0] = (unsigned char)v; b[1] = (unsigned char)(v >> 8U);
}
static void Put32(unsigned char *b, uint32_t v)
{
    for (unsigned i = 0U; i < 4U; ++i) b[i] = (unsigned char)(v >> (8U * i));
}
static void Put64(unsigned char *b, uint64_t v)
{
    Put32(b, (uint32_t)v); Put32(b + 4U, (uint32_t)(v >> 32U));
}
static void Header(unsigned char *b, uint32_t count)
{
    static const unsigned char magic[8] = {'U','M','I','S','C','H','1',0};
    memcpy(b, magic, 8U);
    Put16(b + 8U, 1U); Put16(b + 10U, 0U); Put32(b + 12U, count);
}
static size_t Record(unsigned char *b, uint32_t order, unsigned event,
                     uint32_t task, uint32_t detail, uint64_t tick)
{
    Put32(b, order);
    b[4] = (unsigned char)event; b[5] = 0U;
    Put16(b + 6U, 0U); Put32(b + 8U, task); Put32(b + 12U, detail);
    Put64(b + 16U, tick);
    return UMI_EDUCATION_SCHEDULE_RECORD_BYTES;
}
static int Refused(const unsigned char *bytes, size_t length)
{
    UmiEducationScheduleReport report;
    memset(&report, 0xA5, sizeof(report));
    UmiEducationScheduleReport prior = report;
    const UmiStatus status = UmiEducationScheduleTraceInspect(bytes, length, &report);
    return status != UMI_STATUS_OK && memcmp(&report, &prior, sizeof(report)) == 0;
}
static size_t Standard(unsigned char *bytes)
{
    const struct Step { unsigned event; uint32_t task; uint32_t detail; } plan[] = {
        {1U,10U,0U}, {1U,20U,0U}, {2U,10U,0U}, {4U,10U,7U},
        {2U,20U,0U}, {3U,20U,0U}, {5U,10U,7U}, {2U,20U,0U},
        {6U,20U,0U}, {2U,10U,0U}, {6U,10U,0U}
    };
    Header(bytes, (uint32_t)(sizeof(plan) / sizeof(plan[0])));
    size_t used = UMI_EDUCATION_SCHEDULE_HEADER_BYTES;
    for (uint32_t i = 0U; i < (uint32_t)(sizeof(plan)/sizeof(plan[0])); ++i)
        used += Record(bytes + used, i + 1U, plan[i].event, plan[i].task,
                       plan[i].detail, 10U + i / 2U);
    return used;
}

static int CheckValidAndFixture(const char *fixture)
{
    unsigned char source[UMI_EDUCATION_SCHEDULE_MAX_FILE_BYTES + 1U] = {0};
    const size_t count = Standard(source);
    CHECK(count == 16U + 11U * 24U);
    UmiEducationScheduleReport report = {0};
    CHECK(UmiEducationScheduleTraceInspect(source, count, &report) == UMI_STATUS_OK);
    CHECK(report.recordCount == 11U && report.admitted == 2U &&
          report.dispatched == 4U && report.waited == 1U &&
          report.awakened == 1U && report.yielded == 1U &&
          report.exited == 2U && report.cancelled == 0U &&
          report.readyPeak == 2U && report.waitingPeak == 1U &&
          report.readyAtEnd == 0U && report.waitingAtEnd == 0U &&
          report.liveAtEnd == 0U && report.runningAtEnd == 0U &&
          report.lastTick == 15U);
    /* Save first: the CLI fixture remains the known-good trace after all
     * destructive fuzz trials. Paths containing spaces are part of CTest. */
    FILE *out = fopen(fixture, "wb");
    CHECK(out != NULL);
    CHECK(fwrite(source, 1U, count, out) == count);
    CHECK(fclose(out) == 0);
    UmiEducationScheduleReport loaded = {0};
    CHECK(UmiEducationScheduleTraceInspectFile(fixture, &loaded) == UMI_STATUS_OK);
    CHECK(loaded.recordCount == report.recordCount && loaded.lastTick == report.lastTick);
    return 0;
}
static int CheckMalformed(void)
{
    unsigned char bytes[UMI_EDUCATION_SCHEDULE_MAX_FILE_BYTES + 1U] = {0};
    const size_t count = Standard(bytes);
    for (size_t shortCount = 0U; shortCount < count; ++shortCount)
        CHECK(Refused(bytes, shortCount));
    CHECK(Refused(bytes, count + 1U));
    CHECK(Refused(bytes, UMI_EDUCATION_SCHEDULE_MAX_FILE_BYTES + 1U));
    bytes[0] = 'X'; CHECK(Refused(bytes, count)); bytes[0] = 'U';
    bytes[8] = 2U; CHECK(Refused(bytes, count)); bytes[8] = 1U;
    bytes[10] = 1U; CHECK(Refused(bytes, count)); bytes[10] = 0U;
    Put32(bytes + 12U, UMI_EDUCATION_SCHEDULE_MAX_RECORDS + 1U);
    CHECK(Refused(bytes, count)); Put32(bytes + 12U, 11U);
    /* Wrong FIFO task, wrong wait key, double dispatch and time reversal. */
    Put32(bytes + 16U + 2U * 24U + 8U, 20U);
    CHECK(Refused(bytes, count)); Put32(bytes + 16U + 2U * 24U + 8U, 10U);
    Put32(bytes + 16U + 6U * 24U + 12U, 8U);
    CHECK(Refused(bytes, count)); Put32(bytes + 16U + 6U * 24U + 12U, 7U);
    bytes[16U + 3U * 24U + 4U] = 2U;
    CHECK(Refused(bytes, count)); bytes[16U + 3U * 24U + 4U] = 4U;
    Put64(bytes + 16U + 6U * 24U + 16U, 1U);
    CHECK(Refused(bytes, count)); Put64(bytes + 16U + 6U * 24U + 16U, 13U);
    bytes[16U + 5U] = 1U; CHECK(Refused(bytes, count)); bytes[16U + 5U] = 0U;
    Put16(bytes + 16U + 6U, 1U); CHECK(Refused(bytes, count));
    Put16(bytes + 16U + 6U, 0U);
    Put32(bytes + 16U + 8U, 0U); CHECK(Refused(bytes, count));
    Put32(bytes + 16U + 8U, 10U);
    Put32(bytes + 16U + 24U, 1U); CHECK(Refused(bytes, count));
    Put32(bytes + 16U + 24U, 2U);
    CHECK(UmiEducationScheduleTraceInspect(bytes, count, &(UmiEducationScheduleReport){0}) == UMI_STATUS_OK);
    /* Cancellation is valid only for the same pending wait key. */
    Header(bytes, 4U); size_t used = 16U;
    used += Record(bytes + used, 1U, 1U, 30U, 0U, 1U);
    used += Record(bytes + used, 2U, 2U, 30U, 0U, 2U);
    used += Record(bytes + used, 3U, 4U, 30U, 9U, 3U);
    used += Record(bytes + used, 4U, 7U, 30U, 9U, 4U);
    UmiEducationScheduleReport cancelled = {0};
    CHECK(UmiEducationScheduleTraceInspect(bytes, used, &cancelled) == UMI_STATUS_OK);
    CHECK(cancelled.cancelled == 1U && cancelled.waitingAtEnd == 0U && cancelled.liveAtEnd == 0U);
    Put32(bytes + 16U + 3U * 24U + 12U, 8U);
    CHECK(Refused(bytes, used));
    /* The record reader must survive corrupted input without using bytes
     * outside its complete bounded record. Some mutations remain valid. */
    Standard(bytes);
    uint32_t seed = UINT32_C(0x15e6ca71);
    for (unsigned i = 0U; i < 20000U; ++i) {
        seed = seed * UINT32_C(1664525) + UINT32_C(1013904223);
        size_t position = (size_t)(seed % (uint32_t)count);
        unsigned char saved = bytes[position];
        bytes[position] ^= (unsigned char)((seed >> 16U) | 1U);
        UmiEducationScheduleReport fuzz = {0};
        (void)UmiEducationScheduleTraceInspect(bytes, count, &fuzz);
        bytes[position] = saved;
    }
    CHECK(UmiEducationScheduleTraceInspect(bytes, count, &cancelled) == UMI_STATUS_OK);
    return 0;
}
static int CheckFullQueue(void)
{
    unsigned char bytes[UMI_EDUCATION_SCHEDULE_MAX_FILE_BYTES + 1U] = {0};
    const uint32_t admitted = UMI_EDUCATION_SCHEDULE_MAX_TASKS;
    const uint32_t rotations = 100U;
    const uint32_t recordCount = admitted + rotations * 2U;
    Header(bytes, recordCount);
    size_t used = 16U;
    uint32_t sequence = 1U;
    for (uint32_t i = 1U; i <= admitted; ++i) {
        used += Record(bytes + used, sequence, 1U, i, 0U, sequence);
        ++sequence;
    }
    for (uint32_t i = 0U; i < rotations; ++i) {
        const uint32_t identity = i % admitted + 1U;
        used += Record(bytes + used, sequence, 2U, identity, 0U, sequence);
        ++sequence;
        used += Record(bytes + used, sequence, 3U, identity, 0U, sequence);
        ++sequence;
    }
    UmiEducationScheduleReport report = {0};
    CHECK(UmiEducationScheduleTraceInspect(bytes, used, &report) == UMI_STATUS_OK);
    CHECK(report.readyPeak == admitted && report.readyAtEnd == admitted &&
          report.dispatched == rotations && report.yielded == rotations);
    /* Slots are identities, not reusable generation-free task handles. */
    Header(bytes, admitted + 1U);
    size_t size = 16U;
    for (uint32_t i = 1U; i <= admitted + 1U; ++i)
        size += Record(bytes + size, i, 1U, i, 0U, i);
    CHECK(UmiEducationScheduleTraceInspect(bytes, size, &report) == UMI_STATUS_CAPACITY_EXCEEDED);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2 || argv[1][0] == '\0') return 2;
    CHECK(CheckValidAndFixture(argv[1]) == 0);
    CHECK(CheckMalformed() == 0);
    CHECK(CheckFullQueue() == 0);
    puts("[OK] Scheduling trace: FIFO, waits/wake keys, cancellation and 20000 mutations.");
    return 0;
}
