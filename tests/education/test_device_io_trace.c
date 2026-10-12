/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education/test_device_io_trace.c
 * PURPOSE: Exercise a complete device I/O teaching trace, rejection cases,
 *          failure-atomic reporting and paths containing spaces.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/device_io_trace.h"
#include <stdio.h>
#include <string.h>

#define COUNT 27U
#define TOTAL (UMI_EDUCATION_DEVICE_IO_HEADER_BYTES + \
               COUNT * UMI_EDUCATION_DEVICE_IO_RECORD_BYTES)
#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "[FAIL] %s:%d: %s\n", __FILE__, __LINE__, #condition); \
        return 1; \
    } \
} while (0)

static void Put16(unsigned char *bytes, uint16_t v)
{
    for (unsigned i = 0U; i < 2U; ++i) bytes[i] = (unsigned char)(v >> (i * 8U));
}
static void Put32(unsigned char *bytes, uint32_t v)
{
    for (unsigned i = 0U; i < 4U; ++i) bytes[i] = (unsigned char)(v >> (i * 8U));
}
static void Put64(unsigned char *bytes, uint64_t v)
{
    for (unsigned i = 0U; i < 8U; ++i) bytes[i] = (unsigned char)(v >> (i * 8U));
}
static unsigned char *At(unsigned char *bytes, size_t index)
{
    return bytes + UMI_EDUCATION_DEVICE_IO_HEADER_BYTES +
           index * UMI_EDUCATION_DEVICE_IO_RECORD_BYTES;
}
static void Event(unsigned char *bytes, uint32_t n, unsigned kind, unsigned outcome,
                  unsigned slot, uint32_t owner, uint32_t generation,
                  uint32_t requestId, uint32_t transferred)
{
    unsigned char *p = At(bytes, n - 1U);
    Put32(p, n);
    p[4U] = (unsigned char)kind;
    p[5U] = (unsigned char)outcome;
    Put32(p + 8U, slot);
    Put32(p + 12U, owner);
    Put32(p + 16U, generation);
    Put32(p + 20U, requestId);
    Put32(p + 24U, transferred);
    Put64(p + 28U, n);
}
static void Create(unsigned char *bytes)
{
    const unsigned char magic[8] = {'U','M','I','D','I','O','1',0};
    memset(bytes, 0, TOTAL);
    memcpy(bytes, magic, sizeof(magic));
    Put16(bytes + 8U, 1U);
    Put32(bytes + 12U, COUNT);
    Event(bytes,  1U, 1U, 0U, 1U, 11U, 1U,   0U,   0U); /* attach */
    Event(bytes,  2U, 1U, 0U, 2U, 22U, 1U,   0U,   0U);
    Event(bytes,  3U, 2U, 0U, 1U, 11U, 1U, 100U, 100U); /* submit */
    Event(bytes,  4U, 2U, 0U, 1U, 11U, 1U, 101U,  64U);
    Event(bytes,  5U, 3U, 0U, 1U, 11U, 1U, 101U,  64U); /* out-of-order completion */
    Event(bytes,  6U, 2U, 0U, 2U, 22U, 1U, 102U,  32U);
    Event(bytes,  7U, 4U, 0U, 1U, 11U, 1U, 100U,   0U); /* cancel intent */
    Event(bytes,  8U, 3U, 2U, 1U, 11U, 1U, 100U,   0U); /* cancel ack */
    Event(bytes,  9U, 5U, 0U, 1U, 11U, 1U, 101U,   0U); /* reap in completion order */
    Event(bytes, 10U, 5U, 0U, 1U, 11U, 1U, 100U,   0U);
    Event(bytes, 11U, 6U, 0U, 1U, 11U, 1U,   0U,   0U); /* quiesce */
    Event(bytes, 12U, 2U, 0U, 2U, 22U, 1U, 103U,  56U);
    Event(bytes, 13U, 3U, 1U, 2U, 22U, 1U, 103U,   0U); /* failed completion */
    Event(bytes, 14U, 3U, 0U, 2U, 22U, 1U, 102U,  32U);
    Event(bytes, 15U, 5U, 0U, 2U, 22U, 1U, 103U,   0U);
    Event(bytes, 16U, 5U, 0U, 2U, 22U, 1U, 102U,   0U);
    Event(bytes, 17U, 7U, 0U, 1U, 11U, 1U,   0U,   0U); /* resume */
    Event(bytes, 18U, 2U, 0U, 1U, 11U, 1U, 104U,  16U);
    Event(bytes, 19U, 3U, 0U, 1U, 11U, 1U, 104U,  16U);
    Event(bytes, 20U, 5U, 0U, 1U, 11U, 1U, 104U,   0U);
    Event(bytes, 21U, 6U, 0U, 1U, 11U, 1U,   0U,   0U);
    Event(bytes, 22U, 8U, 0U, 1U, 11U, 1U,   0U,   0U); /* detach */
    Event(bytes, 23U, 6U, 0U, 2U, 22U, 1U,   0U,   0U);
    Event(bytes, 24U, 8U, 0U, 2U, 22U, 1U,   0U,   0U);
    Event(bytes, 25U, 1U, 0U, 1U, 33U, 2U,   0U,   0U); /* reuse slot, new gen */
    Event(bytes, 26U, 6U, 0U, 1U, 33U, 2U,   0U,   0U);
    Event(bytes, 27U, 8U, 0U, 1U, 33U, 2U,   0U,   0U);
}
static int Rejected(const unsigned char *bytes, size_t length)
{
    UmiEducationDeviceIoReport old, expected;
    memset(&old, 0xa5, sizeof(old));
    memcpy(&expected, &old, sizeof(expected));
    UmiStatus status = UmiEducationDeviceIoTraceInspect(bytes, length, &old);
    return status != UMI_STATUS_OK && memcmp(&old, &expected, sizeof(expected)) == 0;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2 && argv[1] != NULL);
    unsigned char valid[TOTAL], mutation[TOTAL];
    Create(valid);
    UmiEducationDeviceIoReport report = {0};
    CHECK(UmiEducationDeviceIoTraceInspect(valid, TOTAL, &report) == UMI_STATUS_OK);
    CHECK(report.recordCount == COUNT && report.attachments == 3U);
    CHECK(report.submissions == 5U && report.completions == 5U && report.reaped == 5U);
    CHECK(report.cancellationRequests == 1U && report.cancellationAcknowledgements == 1U);
    CHECK(report.succeeded == 3U && report.failed == 1U);
    CHECK(report.quiesced == 4U && report.resumed == 1U && report.detached == 3U);
    CHECK(report.peakOutstanding == 3U && report.peakReadyCompletions == 2U);
    CHECK(report.bytesRequested == 268U && report.bytesCompleted == 112U);
    CHECK(report.outstandingAtEnd == 0U && report.readyCompletionsAtEnd == 0U);
    CHECK(report.liveDevicesAtEnd == 0U && report.lastTick == COUNT);
    CHECK(Rejected(valid, TOTAL - 1U));
    CHECK(Rejected(valid, UMI_EDUCATION_DEVICE_IO_HEADER_BYTES - 1U));
    CHECK(UmiEducationDeviceIoTraceInspect(NULL, TOTAL, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEducationDeviceIoTraceInspect(valid, TOTAL, NULL) == UMI_STATUS_INVALID_ARGUMENT);

    memcpy(mutation, valid, TOTAL);
    Put32(mutation + 12U, UINT32_MAX);
    CHECK(Rejected(mutation, TOTAL));
    memcpy(mutation, valid, TOTAL);
    Put32(At(mutation, 8U) + 20U, 100U); /* reap newer completion out of order */
    CHECK(Rejected(mutation, TOTAL));
    memcpy(mutation, valid, TOTAL);
    At(mutation, 6U)[4U] = 3U; /* complete as success in place of cancel */
    CHECK(Rejected(mutation, TOTAL));
    memcpy(mutation, valid, TOTAL);
    At(mutation, 7U)[5U] = 2U;
    At(mutation, 6U)[4U] = 7U; /* no cancel request precedes cancelled outcome */
    CHECK(Rejected(mutation, TOTAL));
    memcpy(mutation, valid, TOTAL);
    Put32(At(mutation, 24U) + 16U, 1U); /* stale generation on reattach */
    CHECK(Rejected(mutation, TOTAL));
    memcpy(mutation, valid, TOTAL);
    Put32(At(mutation, 17U) + 24U, 4097U);
    CHECK(Rejected(mutation, TOTAL));
    memcpy(mutation, valid, TOTAL);
    At(mutation, 20U)[4U] = 8U; /* detach without quiescing */
    CHECK(Rejected(mutation, TOTAL));
    memcpy(mutation, valid, TOTAL);
    At(mutation, 9U)[4U] = 8U; /* detach with requests pending */
    CHECK(Rejected(mutation, TOTAL));
    memcpy(mutation, valid, TOTAL);
    Put64(At(mutation, 10U) + 28U, 1U); /* clock rollback */
    CHECK(Rejected(mutation, TOTAL));
    memcpy(mutation, valid, TOTAL);
    At(mutation, 0U)[5U] = 1U; /* outcome only valid at completion */
    CHECK(Rejected(mutation, TOTAL));
    memcpy(mutation, valid, TOTAL);
    At(mutation, 0U)[36U] = 1U; /* reserved bits */
    CHECK(Rejected(mutation, TOTAL));

    /* Mutate a field guaranteed to be meaningful in every record; success
     * is never a valid result for a nonexistent event number 255. */
    for (uint32_t seed = 0U; seed < 20000U; ++seed) {
        memcpy(mutation, valid, TOTAL);
        const size_t index = (size_t)((seed * 13U + 7U) % COUNT);
        At(mutation, index)[4U] = 255U;
        CHECK(Rejected(mutation, TOTAL));
    }

    /* Four completed-but-uncollected results use every completion slot. A
     * fifth device result cannot replace an earlier accepted completion. */
    unsigned char full[UMI_EDUCATION_DEVICE_IO_HEADER_BYTES +
                       11U * UMI_EDUCATION_DEVICE_IO_RECORD_BYTES] = {0};
    const unsigned char fullMagic[8] = {'U','M','I','D','I','O','1',0};
    memcpy(full, fullMagic, sizeof(fullMagic));
    Put16(full + 8U, 1U);
    Put32(full + 12U, 11U);
    Event(full, 1U, 1U, 0U, 1U, 11U, 1U, 0U, 0U);
    for (uint32_t id = 1U; id <= 5U; ++id)
        Event(full, id + 1U, 2U, 0U, 1U, 11U, 1U, id, 5U);
    for (uint32_t id = 1U; id <= 5U; ++id)
        Event(full, id + 6U, 3U, 0U, 1U, 11U, 1U, id, 5U);
    CHECK(Rejected(full, sizeof(full)));
    Put32(full + 12U, 10U);
    UmiEducationDeviceIoReport partial = {0};
    CHECK(UmiEducationDeviceIoTraceInspect(
        full, sizeof(full)-UMI_EDUCATION_DEVICE_IO_RECORD_BYTES,
        &partial) == UMI_STATUS_OK);
    CHECK(partial.outstandingAtEnd == 5U && partial.readyCompletionsAtEnd == 4U);
    CHECK(partial.liveDevicesAtEnd == 1U);

    FILE *file = fopen(argv[1], "wb");
    CHECK(file != NULL);
    CHECK(fwrite(valid, 1U, TOTAL, file) == TOTAL);
    CHECK(fclose(file) == 0);
    memset(&report, 0, sizeof(report));
    CHECK(UmiEducationDeviceIoTraceInspectFile(argv[1], &report) == UMI_STATUS_OK);
    CHECK(report.recordCount == COUNT && report.bytesCompleted == 112U);
    puts("[OK] Device I/O trace: lifecycle, FIFO completion order, cancellation, "
         "bounds, spaced file paths and 20,000 damaged inputs.");
    return 0;
}
