/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_provenance_release.c
 *
 * PURPOSE:
 *   Verify the delivery-platform behaviour exercised by this focused test.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This small executable uses assertions so a failure points directly at one delivery contract.
 */

/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include "umicom/delivery/provenance.h"
#include "umicom/delivery/release.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "value_archive/transfer_cases.h"

#include "umicom/delivery/provenance.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiProvenanceTransferEqual(const UmiProvenance *a, const UmiProvenance *b)
{
    return strcmp(a->source_revision, b->source_revision) == 0 &&
        strcmp(a->builder_id, b->builder_id) == 0 &&
        strcmp(a->build_preset, b->build_preset) == 0 &&
        strcmp(a->framework_version, b->framework_version) == 0 &&
        a->created_epoch_ms == b->created_epoch_ms;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiProvenanceTransferTails(UmiProvenance *value)
{
    (void)value;
    {
        size_t used = strlen(value->source_revision) + 1U;
        memset(value->source_revision + used, 0xa5, sizeof(value->source_revision) - used);
    }
    {
        size_t used = strlen(value->builder_id) + 1U;
        memset(value->builder_id + used, 0xa5, sizeof(value->builder_id) - used);
    }
    {
        size_t used = strlen(value->build_preset) + 1U;
        memset(value->build_preset + used, 0xa5, sizeof(value->build_preset) - used);
    }
    {
        size_t used = strlen(value->framework_version) + 1U;
        memset(value->framework_version + used, 0xa5, sizeof(value->framework_version) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiProvenanceTransferMalformed(const UmiProvenance *sample)
{
    (void)sample;
    {
        UmiProvenance invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_revision, 'x', sizeof(invalid.source_revision));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_provenance_validate(&invalid) != UMI_STATUS_OK) ||
            umi_provenance_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_revision was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiProvenance invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.builder_id, 'x', sizeof(invalid.builder_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_provenance_validate(&invalid) != UMI_STATUS_OK) ||
            umi_provenance_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated builder_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiProvenance invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.build_preset, 'x', sizeof(invalid.build_preset));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_provenance_validate(&invalid) != UMI_STATUS_OK) ||
            umi_provenance_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated build_preset was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiProvenance invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.framework_version, 'x', sizeof(invalid.framework_version));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_provenance_validate(&invalid) != UMI_STATUS_OK) ||
            umi_provenance_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated framework_version was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiProvenanceTransferCases, UmiProvenance,
    umi_provenance_archive_encode, umi_provenance_archive_decode,
    UmiProvenanceTransferEqual, UmiProvenanceTransferTails, UmiProvenanceTransferMalformed)

int main(void) {
    UmiProvenance p;
    UmiDeliveryManifest m;
    UmiRelease r;
    assert(umi_provenance_init(&p, "abc", "builder", "debug") == UMI_STATUS_OK);
    assert(umi_provenance_validate(&p) == UMI_STATUS_OK);
    if (UmiProvenanceTransferCases(&p) != 0) return 1;

    assert(umi_delivery_manifest_init(&m, "studio", "rel", "0.14.0", UMI_RELEASE_BETA) == UMI_STATUS_OK);
    assert(umi_release_init(&r, &m, 7U) == UMI_STATUS_OK);
    r.verification = UMI_EVIDENCE_PASS;
    assert(umi_release_ready_to_publish(&r));
    return 0;
}
