/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_manifest_evidence.c
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
#include "umicom/delivery/manifest.h"
#include "umicom/delivery/build_evidence.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "value_archive/transfer_cases.h"

#include "umicom/delivery/manifest.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDeliveryManifestTransferEqual(const UmiDeliveryManifest *a, const UmiDeliveryManifest *b)
{
    return strcmp(a->application_id, b->application_id) == 0 &&
        strcmp(a->release_id, b->release_id) == 0 &&
        strcmp(a->version, b->version) == 0 &&
        strcmp(a->generation_id, b->generation_id) == 0 &&
        strcmp(a->source_revision, b->source_revision) == 0 &&
        a->channel == b->channel &&
        a->created_epoch_ms == b->created_epoch_ms &&
        a->artifact_count == b->artifact_count;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDeliveryManifestTransferTails(UmiDeliveryManifest *value)
{
    (void)value;
    {
        size_t used = strlen(value->application_id) + 1U;
        memset(value->application_id + used, 0xa5, sizeof(value->application_id) - used);
    }
    {
        size_t used = strlen(value->release_id) + 1U;
        memset(value->release_id + used, 0xa5, sizeof(value->release_id) - used);
    }
    {
        size_t used = strlen(value->version) + 1U;
        memset(value->version + used, 0xa5, sizeof(value->version) - used);
    }
    {
        size_t used = strlen(value->generation_id) + 1U;
        memset(value->generation_id + used, 0xa5, sizeof(value->generation_id) - used);
    }
    {
        size_t used = strlen(value->source_revision) + 1U;
        memset(value->source_revision + used, 0xa5, sizeof(value->source_revision) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDeliveryManifestTransferMalformed(const UmiDeliveryManifest *sample)
{
    (void)sample;
    {
        UmiDeliveryManifest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.application_id, 'x', sizeof(invalid.application_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_delivery_manifest_validate(&invalid) != UMI_STATUS_OK) ||
            umi_delivery_manifest_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated application_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDeliveryManifest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.release_id, 'x', sizeof(invalid.release_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_delivery_manifest_validate(&invalid) != UMI_STATUS_OK) ||
            umi_delivery_manifest_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated release_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDeliveryManifest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.version, 'x', sizeof(invalid.version));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_delivery_manifest_validate(&invalid) != UMI_STATUS_OK) ||
            umi_delivery_manifest_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated version was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDeliveryManifest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.generation_id, 'x', sizeof(invalid.generation_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_delivery_manifest_validate(&invalid) != UMI_STATUS_OK) ||
            umi_delivery_manifest_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated generation_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDeliveryManifest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_revision, 'x', sizeof(invalid.source_revision));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_delivery_manifest_validate(&invalid) != UMI_STATUS_OK) ||
            umi_delivery_manifest_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_revision was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDeliveryManifestTransferCases, UmiDeliveryManifest,
    umi_delivery_manifest_archive_encode, umi_delivery_manifest_archive_decode,
    UmiDeliveryManifestTransferEqual, UmiDeliveryManifestTransferTails, UmiDeliveryManifestTransferMalformed)

int main(void) {
    UmiDeliveryManifest manifest;
    UmiBuildEvidence evidence;
    assert(umi_delivery_manifest_init(&manifest, "studio", "r1", "0.14.0", UMI_RELEASE_DEVELOPMENT) == UMI_STATUS_OK);
    assert(umi_delivery_manifest_validate(&manifest) == UMI_STATUS_OK);
    if (UmiDeliveryManifestTransferCases(&manifest) != 0) return 1;

    umi_build_evidence_init(&evidence);
    evidence.build_succeeded = 1;
    evidence.tests_total = 5U;
    evidence.tests_passed = 5U;
    assert(umi_build_evidence_passed(&evidence));
    return 0;
}
