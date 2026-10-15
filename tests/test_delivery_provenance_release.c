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
#include "value_archive/construction_cases.h"
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

#include "value_archive/conditional_cases.h"
UMI_TEST_CONDITIONAL_ARCHIVE(UmiProvenanceConditionalArchiveCases, UmiProvenance,
    umi_provenance_archive_encode, umi_provenance_archive_decode_if_unchanged,
    UmiProvenanceTransferEqual, UmiProvenanceTransferTails, UmiProvenanceTransferMalformed)



/* A rejected replacement must preserve the previously accepted provenance value. */
static int UmiProvenanceConstructionCases(void)
{
    UmiProvenance *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_provenance_init(value, "abc", "builder", "debug") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->source_revision) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_provenance_init(value, oversized, "builder", "debug") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_provenance_init(value, value->source_revision, "builder", "debug") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->source_revision, "abc") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->builder_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_provenance_init(value, "abc", oversized, "debug") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_provenance_init(value, "abc", value->builder_id, "debug") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->builder_id, "builder") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->build_preset) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_provenance_init(value, "abc", "builder", oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_provenance_init(value, "abc", "builder", value->build_preset) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->build_preset, "debug") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiProvenanceConstructionCases() != 0) return 1;

    UmiProvenance p;
    UmiDeliveryManifest m;
    UmiRelease r;
    assert(umi_provenance_init(&p, "abc", "builder", "debug") == UMI_STATUS_OK);
    assert(umi_provenance_validate(&p) == UMI_STATUS_OK);
    if (UmiProvenanceTransferCases(&p) != 0) return 1;
    if (UmiProvenanceConditionalArchiveCases(&p) != 0) return 1;

    assert(umi_delivery_manifest_init(&m, "studio", "rel", "0.14.0", UMI_RELEASE_BETA) == UMI_STATUS_OK);
    assert(umi_release_init(&r, &m, 7U) == UMI_STATUS_OK);
    r.verification = UMI_EVIDENCE_PASS;
    assert(umi_release_ready_to_publish(&r));
    return 0;
}
