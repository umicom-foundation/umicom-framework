/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_runtime_probe.c
 *
 * PURPOSE:
 *   Focused regression coverage for deterministic probe snapshots describing detected host runtime properties.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/distribution/runtime/runtime_probe.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/runtime_probe.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrRuntimeProbeTransferEqual(const UmiDrRuntimeProbe *a, const UmiDrRuntimeProbe *b)
{
    return strcmp(a->id, b->id) == 0 &&
        a->platform == b->platform &&
        a->architecture == b->architecture &&
        a->version.major == b->version.major &&
        a->version.minor == b->version.minor &&
        a->version.patch == b->version.patch &&
        a->capabilities == b->capabilities &&
        a->memory_mb == b->memory_mb;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrRuntimeProbeTransferTails(UmiDrRuntimeProbe *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDrRuntimeProbeTransferMalformed(const UmiDrRuntimeProbe *sample)
{
    (void)sample;
    {
        UmiDrRuntimeProbe invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_runtime_probe_valid(&invalid)) ||
            umi_dr_runtime_probe_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrRuntimeProbeTransferCases, UmiDrRuntimeProbe,
    umi_dr_runtime_probe_archive_encode, umi_dr_runtime_probe_archive_decode,
    UmiDrRuntimeProbeTransferEqual, UmiDrRuntimeProbeTransferTails, UmiDrRuntimeProbeTransferMalformed)

int main(void) {
    UmiDrRuntimeProbe value; umi_dr_runtime_probe_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"host") == UMI_STATUS_OK); value.memory_mb=16384U; CHECK(umi_dr_runtime_probe_valid(&value));
    if (UmiDrRuntimeProbeTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_runtime_probe_fingerprint(&value) != 0U);
    return 0;
}
