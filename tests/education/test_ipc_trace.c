/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education/test_ipc_trace.c
 * PURPOSE: Test valid, malformed, oversized and stale IPC trace observations;
 *          create a spaced-path fixture for the independent CLI regression.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/ipc_trace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        (void)fprintf(stderr, "[FAIL] %s:%d: %s\n", __FILE__, __LINE__, #condition); \
        return 1; \
    } \
} while (0)

static void Put16(unsigned char *p, uint16_t v)
{
    p[0] = (unsigned char)v;
    p[1] = (unsigned char)(v >> 8U);
}
static void Put32(unsigned char *p, uint32_t v)
{
    for (size_t i = 0U; i < 4U; ++i)
        p[i] = (unsigned char)(v >> (i * 8U));
}
static void Header(unsigned char *p, uint32_t records)
{
    static const unsigned char magic[8] = {'U','M','I','I','P','C','1',0};
    memcpy(p, magic, 8U);
    Put16(p + 8U, 1U);
    Put16(p + 10U, 0U);
    Put32(p + 12U, records);
}
static size_t Record(unsigned char *p, uint32_t sequence, unsigned char kind,
                     uint32_t sender, uint32_t recipient,
                     const unsigned char *payload, uint16_t length)
{
    Put32(p, sequence);
    p[4] = kind;
    p[5] = 0U;
    Put16(p + 6U, length);
    Put32(p + 8U, sender);
    Put32(p + 12U, recipient);
    if (length != 0U)
        memcpy(p + 16U, payload, (size_t)length);
    return 16U + (size_t)length;
}

/* The unchanged-on-error sentinel catches partial publication of a report. */
static int Rejected(const unsigned char *data, size_t size)
{
    UmiEducationIpcTraceReport report;
    memset(&report, 0xA5, sizeof(report));
    UmiEducationIpcTraceReport original = report;
    UmiStatus status = UmiEducationIpcTraceInspect(data, size, &report);
    return status != UMI_STATUS_OK && report.recordCount == original.recordCount &&
        report.sendCount == original.sendCount && report.receiveCount == original.receiveCount &&
        report.pendingCount == original.pendingCount && report.highWater == original.highWater &&
        report.payloadBytes == original.payloadBytes;
}

static int Run(const char *fixture)
{
    const unsigned char a[] = {'A'};
    const unsigned char b[] = {'N', 'o', 't', 'e'};
    unsigned char bytes[9000] = {0};
    Header(bytes, 4U);
    size_t size = 16U;
    size += Record(bytes + size, 1U, 1U, 10U, 20U, a, 1U);
    size += Record(bytes + size, 2U, 1U, 30U, 40U, b, 4U);
    size += Record(bytes + size, 3U, 2U, 10U, 20U, NULL, 0U);
    size += Record(bytes + size, 4U, 2U, 30U, 40U, NULL, 0U);
    const size_t complete = size;
    UmiEducationIpcTraceReport report = {0};
    CHECK(UmiEducationIpcTraceInspect(bytes, size, &report) == UMI_STATUS_OK);
    CHECK(report.recordCount == 4U && report.sendCount == 2U &&
          report.receiveCount == 2U && report.pendingCount == 0U &&
          report.highWater == 2U && report.payloadBytes == 5U);
    CHECK(Rejected(bytes, 15U));
    for (size_t shortSize = 0U; shortSize < size; ++shortSize)
        CHECK(Rejected(bytes, shortSize));
    CHECK(Rejected(bytes, size - 1U));
    CHECK(Rejected(bytes, size + 1U));
    bytes[0] = 'X'; CHECK(Rejected(bytes, size)); bytes[0] = 'U';
    bytes[8] = 2U; CHECK(Rejected(bytes, size)); bytes[8] = 1U;
    bytes[10] = 1U; CHECK(Rejected(bytes, size)); bytes[10] = 0U;
    bytes[16U + 4U] = 3U; CHECK(Rejected(bytes, size)); bytes[16U + 4U] = 1U;
    bytes[16U + 5U] = 1U; CHECK(Rejected(bytes, size)); bytes[16U + 5U] = 0U;
    bytes[16U + 6U] = 0U; CHECK(Rejected(bytes, size)); bytes[16U + 6U] = 1U;
    bytes[16U] = 2U; CHECK(Rejected(bytes, size)); bytes[16U] = 1U;
    /* The first receive must select the oldest outstanding send. */
    const size_t firstReceive = 16U + 17U + 20U;
    Put32(bytes + firstReceive + 8U, 30U);
    CHECK(Rejected(bytes, size));
    Put32(bytes + firstReceive + 8U, 10U);
    /* Incomplete declared count, invalid IDs and receive-with-payload. */
    Put32(bytes + 12U, 5U); CHECK(Rejected(bytes, size)); Put32(bytes + 12U, 4U);
    Put32(bytes + 12U, UMI_EDUCATION_IPC_TRACE_MAX_RECORDS + 1U);
    CHECK(UmiEducationIpcTraceInspect(bytes, size, &report) == UMI_STATUS_CAPACITY_EXCEEDED);
    Put32(bytes + 12U, 4U);
    CHECK(UmiEducationIpcTraceInspect(bytes, UMI_EDUCATION_IPC_TRACE_MAX_BYTES + 1U,
                                    &report) == UMI_STATUS_CAPACITY_EXCEEDED);
    Put32(bytes + 16U + 8U, 0U); CHECK(Rejected(bytes, size)); Put32(bytes + 16U + 8U, 10U);
    Put16(bytes + firstReceive + 6U, 1U); CHECK(Rejected(bytes, size)); Put16(bytes + firstReceive + 6U, 0U);

    /* More than 64 unreceived sends is a capacity refusal, not overwriting. */
    Header(bytes, 65U);
    size = 16U;
    for (uint32_t i = 1U; i <= 65U; ++i)
        size += Record(bytes + size, i, 1U, 10U, 20U, a, 1U);
    CHECK(Rejected(bytes, size));
    CHECK(UmiEducationIpcTraceInspect(bytes, size, &report) == UMI_STATUS_CAPACITY_EXCEEDED);

    /* 20,000 deterministic mutations stay inside a bounded source slice. A
     * randomly corrupted payload may remain valid; success is not an error. */
    Header(bytes, 4U);
    size = 16U;
    size += Record(bytes + size, 1U, 1U, 10U, 20U, a, 1U);
    size += Record(bytes + size, 2U, 1U, 30U, 40U, b, 4U);
    size += Record(bytes + size, 3U, 2U, 10U, 20U, NULL, 0U);
    size += Record(bytes + size, 4U, 2U, 30U, 40U, NULL, 0U);
    CHECK(size == complete);
    uint32_t seed = UINT32_C(0x14a22cc3);
    for (size_t i = 0U; i < 20000U; ++i) {
        seed = seed * UINT32_C(1664525) + UINT32_C(1013904223);
        const size_t position = (size_t)(seed % (uint32_t)size);
        const unsigned char original = bytes[position];
        bytes[position] ^= (unsigned char)((seed >> 16U) | 1U);
        UmiEducationIpcTraceReport sample = {0};
        (void)UmiEducationIpcTraceInspect(bytes, size, &sample);
        bytes[position] = original;
    }
    CHECK(UmiEducationIpcTraceInspect(bytes, size, &report) == UMI_STATUS_OK);
    Header(bytes, 1U);
    size_t one = 16U;
    one += Record(bytes + one, 1U, 1U, 10U, 20U, a, 1U);
    CHECK(UmiEducationIpcTraceInspect(bytes, one, &report) == UMI_STATUS_OK);
    CHECK(report.pendingCount == 1U && report.receiveCount == 0U);
    Header(bytes, 4U);
    size = 16U;
    size += Record(bytes + size, 1U, 1U, 10U, 20U, a, 1U);
    size += Record(bytes + size, 2U, 1U, 30U, 40U, b, 4U);
    size += Record(bytes + size, 3U, 2U, 10U, 20U, NULL, 0U);
    size += Record(bytes + size, 4U, 2U, 30U, 40U, NULL, 0U);

    FILE *file = fopen(fixture, "wb");
    CHECK(file != NULL);
    CHECK(fwrite(bytes, 1U, size, file) == size);
    CHECK(fclose(file) == 0);
    UmiEducationIpcTraceReport loaded = {0};
    CHECK(UmiEducationIpcTraceInspectFile(fixture, &loaded) == UMI_STATUS_OK);
    CHECK(loaded.payloadBytes == 5U && loaded.highWater == 2U);
    puts("[OK] IPC trace: FIFO ownership, malformed inputs and 20000 mutations.");
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2 || argv[1][0] == '\0') {
        fputs("Expected output fixture file path.\n", stderr);
        return 2;
    }
    return Run(argv[1]);
}
