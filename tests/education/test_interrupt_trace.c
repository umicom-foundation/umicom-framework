/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education/test_interrupt_trace.c
 * PURPOSE: Verify strict interrupt ownership, expiry generations, fault review,
 *          report atomicity and literal paths containing spaces.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/interrupt_trace.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "[FAIL] %s:%d: %s\n", __FILE__, __LINE__, #condition); \
    return 1; } } while (0)

static void Put16(unsigned char *p, uint16_t value)
{
    p[0] = (unsigned char)value;
    p[1] = (unsigned char)(value >> 8U);
}
static void Put32(unsigned char *p, uint32_t value)
{
    for (unsigned i = 0U; i < 4U; ++i)
        p[i] = (unsigned char)(value >> (8U * i));
}
static void Put64(unsigned char *p, uint64_t value)
{
    Put32(p, (uint32_t)value);
    Put32(p + 4U, (uint32_t)(value >> 32U));
}
static void Header(unsigned char *p, uint32_t count)
{
    const unsigned char magic[8] = {'U','M','I','I','R','Q','1',0};
    memcpy(p, magic, sizeof(magic));
    Put16(p + 8U, 1U); Put16(p + 10U, 0U); Put32(p + 12U, count);
}
static void Record(unsigned char *p, uint32_t ordinal, unsigned event,
                   uint16_t source, uint32_t owner, uint32_t generation,
                   uint64_t tick, uint64_t detail)
{
    Put32(p, ordinal);
    p[4] = (unsigned char)event;
    p[5] = 0U;
    Put16(p + 6U, source);
    Put32(p + 8U, owner);
    Put32(p + 12U, generation);
    Put64(p + 16U, tick);
    Put64(p + 24U, detail);
}
static size_t Standard(unsigned char *bytes)
{
    static const struct Step {
        unsigned event; uint16_t source; uint32_t owner, generation;
        uint64_t tick, detail;
    } steps[] = {
        {1U,1U,20U,1U, 1U,0U},
        {2U,1U,20U,1U, 1U,0U},
        {3U,1U,20U,1U, 2U,0U},
        {4U,1U,20U,1U, 2U,0U},
        {5U,1U,20U,1U, 3U,0U},
        {2U,1U,20U,1U, 4U,0U},
        {6U,1U,20U,1U, 4U,0U},
        {9U,1U,20U,1U, 4U,0U},
        {1U,1U,21U,2U, 5U,0U},
        {1U,2U,25U,1U, 5U,0U},
        {2U,1U,21U,2U, 6U,0U},
        {3U,1U,21U,2U, 6U,0U},
        {7U,1U,21U,2U, 6U,3U},
        {8U,1U,21U,2U, 7U,0U},
        {9U,1U,21U,2U, 8U,0U},
        {2U,2U,25U,1U, 8U,0U},
        {3U,2U,25U,1U, 9U,0U},
        {4U,2U,25U,1U, 9U,0U},
        {5U,2U,25U,1U,10U,0U},
        {9U,2U,25U,1U,10U,0U}
    };
    const uint32_t count = (uint32_t)(sizeof(steps) / sizeof(steps[0]));
    Header(bytes, count);
    for (uint32_t i = 0U; i < count; ++i)
        Record(bytes + 16U + (size_t)i * 32U, i + 1U, steps[i].event,
               steps[i].source, steps[i].owner, steps[i].generation,
               steps[i].tick, steps[i].detail);
    return 16U + (size_t)count * 32U;
}
/* Keep the entire prior result byte-identical whenever one record is refused. */
static int Refused(const unsigned char *bytes, size_t count)
{
    UmiEducationInterruptReport report;
    memset(&report, 0xa5, sizeof(report));
    const UmiEducationInterruptReport saved = report;
    return UmiEducationInterruptTraceInspect(bytes, count, &report) != UMI_STATUS_OK &&
           memcmp(&report, &saved, sizeof(report)) == 0;
}
static int CheckStandard(const char *fixture)
{
    unsigned char bytes[UMI_EDUCATION_INTERRUPT_MAX_FILE_BYTES + 1U] = {0};
    const size_t length = Standard(bytes);
    UmiEducationInterruptReport report = {0};
    CHECK(UmiEducationInterruptTraceInspect(bytes, length, &report) == UMI_STATUS_OK);
    CHECK(report.recordCount == 20U && report.claims == 3U && report.arms == 4U &&
          report.entries == 3U && report.acknowledgements == 2U &&
          report.returns == 2U && report.cancellations == 1U &&
          report.faults == 1U && report.recoveries == 1U && report.releases == 3U &&
          report.peakOwned == 2U && report.ownedAtEnd == 0U &&
          report.activeAtEnd == 0U && report.faultedAtEnd == 0U &&
          report.lastTick == 10U);
    FILE *file = fopen(fixture, "wb");
    CHECK(file != NULL);
    CHECK(fwrite(bytes, 1U, length, file) == length);
    CHECK(fclose(file) == 0);
    UmiEducationInterruptReport loaded = {0};
    CHECK(UmiEducationInterruptTraceInspectFile(fixture, &loaded) == UMI_STATUS_OK);
    CHECK(loaded.claims == report.claims && loaded.faults == report.faults);
    CHECK(UmiEducationInterruptTraceInspectFile("missing-interrupt-fixture-404", &loaded) != UMI_STATUS_OK);
    return 0;
}
static int CheckInvalid(void)
{
    unsigned char bytes[UMI_EDUCATION_INTERRUPT_MAX_FILE_BYTES + 1U] = {0};
    const size_t length = Standard(bytes);
    for (size_t cut = 0U; cut < length; ++cut) CHECK(Refused(bytes, cut));
    CHECK(Refused(bytes, length + 1U));
    CHECK(Refused(bytes, UMI_EDUCATION_INTERRUPT_MAX_FILE_BYTES + 1U));
    bytes[0] = 'X'; CHECK(Refused(bytes, length)); bytes[0] = 'U';
    bytes[8] = 2U; CHECK(Refused(bytes, length)); bytes[8] = 1U;
    bytes[10] = 1U; CHECK(Refused(bytes, length)); bytes[10] = 0U;
    Put32(bytes + 12U, UMI_EDUCATION_INTERRUPT_MAX_RECORDS + 1U);
    CHECK(Refused(bytes, length)); Put32(bytes + 12U, 20U);
    /* A stale generation, wrong owner or recycled token cannot act. */
    Put32(bytes + 16U + 8U * 32U + 12U, 1U);
    CHECK(Refused(bytes, length)); Put32(bytes + 16U + 8U * 32U + 12U, 2U);
    Put32(bytes + 16U + 10U * 32U + 8U, 20U);
    CHECK(Refused(bytes, length)); Put32(bytes + 16U + 10U * 32U + 8U, 21U);
    /* Acknowledgement cannot occur before ENTER; RETURN requires ACK. */
    bytes[16U + 2U * 32U + 4U] = 4U;
    CHECK(Refused(bytes, length)); bytes[16U + 2U * 32U + 4U] = 3U;
    bytes[16U + 3U * 32U + 4U] = 5U;
    CHECK(Refused(bytes, length)); bytes[16U + 3U * 32U + 4U] = 4U;
    /* You cannot cancel a source already inside its handler. */
    bytes[16U + 3U * 32U + 4U] = 6U;
    CHECK(Refused(bytes, length)); bytes[16U + 3U * 32U + 4U] = 4U;
    /* A fault requires a diagnostic, and recovery requires a recorded fault. */
    Put64(bytes + 16U + 12U * 32U + 24U, 0U);
    CHECK(Refused(bytes, length)); Put64(bytes + 16U + 12U * 32U + 24U, 3U);
    bytes[16U + 11U * 32U + 4U] = 8U;
    CHECK(Refused(bytes, length)); bytes[16U + 11U * 32U + 4U] = 3U;
    /* Reject a source number out of range and backwards time. */
    Put16(bytes + 16U + 6U, 9U); CHECK(Refused(bytes, length));
    Put16(bytes + 16U + 6U, 1U);
    Put64(bytes + 16U + 4U * 32U + 16U, 1U);
    CHECK(Refused(bytes, length)); Put64(bytes + 16U + 4U * 32U + 16U, 3U);
    bytes[16U + 5U] = 1U; CHECK(Refused(bytes, length)); bytes[16U + 5U] = 0U;
    Put32(bytes + 16U + 32U, 45U); CHECK(Refused(bytes, length));
    Put32(bytes + 16U + 32U, 2U);
    /* Byte mutations must not read out of bounds, even when still valid. */
    const size_t standardSize = Standard(bytes);
    uint32_t seed = UINT32_C(0xf5276219);
    for (unsigned i = 0U; i < 20000U; ++i) {
        seed = seed * UINT32_C(1664525) + UINT32_C(1013904223);
        const size_t offset = (size_t)(seed % (uint32_t)standardSize);
        const unsigned char saved = bytes[offset];
        bytes[offset] ^= (unsigned char)((seed >> 17U) | 1U);
        UmiEducationInterruptReport result = {0};
        (void)UmiEducationInterruptTraceInspect(bytes, standardSize, &result);
        bytes[offset] = saved;
    }
    UmiEducationInterruptReport again = {0};
    CHECK(UmiEducationInterruptTraceInspect(bytes, standardSize, &again) == UMI_STATUS_OK);
    return 0;
}
static int CheckCapacity(void)
{
    unsigned char bytes[UMI_EDUCATION_INTERRUPT_MAX_FILE_BYTES + 1U] = {0};
    Header(bytes, 9U);
    for (uint32_t i = 0U; i < 9U; ++i)
        Record(bytes + 16U + (size_t)i * 32U, i + 1U, 1U,
               (uint16_t)(i + 1U), i + 1U, 1U, i, 0U);
    CHECK(Refused(bytes, 16U + 9U * 32U));
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2 || argv[1][0] == '\0') return 2;
    CHECK(CheckStandard(argv[1]) == 0);
    CHECK(CheckInvalid() == 0);
    CHECK(CheckCapacity() == 0);
    puts("[OK] Interrupt trace: ownership, acknowledgement, explicit recovery and 20000 mutations.");
    return 0;
}
