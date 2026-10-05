/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_provenance.c
 *
 * PURPOSE:
 *   Focused regression coverage for build/source/toolchain provenance evidence for packaged releases.
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
#include "umicom/distribution/runtime/provenance.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/provenance.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrProvenanceTransferEqual(const UmiDrProvenance *a, const UmiDrProvenance *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->source_revision, b->source_revision) == 0 &&
        strcmp(a->toolchain, b->toolchain) == 0 &&
        strcmp(a->builder, b->builder) == 0 &&
        a->reproducible == b->reproducible;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrProvenanceTransferTails(UmiDrProvenance *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->source_revision) + 1U;
        memset(value->source_revision + used, 0xa5, sizeof(value->source_revision) - used);
    }
    {
        size_t used = strlen(value->toolchain) + 1U;
        memset(value->toolchain + used, 0xa5, sizeof(value->toolchain) - used);
    }
    {
        size_t used = strlen(value->builder) + 1U;
        memset(value->builder + used, 0xa5, sizeof(value->builder) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDrProvenanceTransferMalformed(const UmiDrProvenance *sample)
{
    (void)sample;
    {
        UmiDrProvenance invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_provenance_valid(&invalid)) ||
            umi_dr_provenance_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrProvenance invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_revision, 'x', sizeof(invalid.source_revision));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_provenance_valid(&invalid)) ||
            umi_dr_provenance_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_revision was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrProvenance invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.toolchain, 'x', sizeof(invalid.toolchain));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_provenance_valid(&invalid)) ||
            umi_dr_provenance_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated toolchain was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrProvenance invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.builder, 'x', sizeof(invalid.builder));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_provenance_valid(&invalid)) ||
            umi_dr_provenance_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated builder was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrProvenanceTransferCases, UmiDrProvenance,
    umi_dr_provenance_archive_encode, umi_dr_provenance_archive_decode,
    UmiDrProvenanceTransferEqual, UmiDrProvenanceTransferTails, UmiDrProvenanceTransferMalformed)

int main(void) {
    UmiDrProvenance value; umi_dr_provenance_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"prov")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.source_revision,sizeof(value.source_revision),"rev")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.toolchain,sizeof(value.toolchain),"gcc-14")==UMI_STATUS_OK); CHECK(umi_dr_provenance_valid(&value));
    if (UmiDrProvenanceTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_provenance_fingerprint(&value) != 0U);
    return 0;
}
