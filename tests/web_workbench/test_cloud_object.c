/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web_workbench/test_cloud_object.c
 * PURPOSE: Verify credential-referenced cloud profiles and object inventory.
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
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/web/workbench/cloud_object.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/web/workbench/cloud_object.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWebWorkbenchCloudProfileTransferEqual(const UmiWebWorkbenchCloudProfile *a, const UmiWebWorkbenchCloudProfile *b)
{
    return strcmp(a->profile_id, b->profile_id) == 0 &&
        strcmp(a->name, b->name) == 0 &&
        a->provider == b->provider &&
        strcmp(a->region, b->region) == 0 &&
        strcmp(a->endpoint, b->endpoint) == 0 &&
        strcmp(a->secret_reference, b->secret_reference) == 0 &&
        a->verify_tls == b->verify_tls &&
        a->enabled == b->enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiWebWorkbenchCloudProfileTransferTails(UmiWebWorkbenchCloudProfile *value)
{
    (void)value;
    {
        size_t used = strlen(value->profile_id) + 1U;
        memset(value->profile_id + used, 0xa5, sizeof(value->profile_id) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
    {
        size_t used = strlen(value->region) + 1U;
        memset(value->region + used, 0xa5, sizeof(value->region) - used);
    }
    {
        size_t used = strlen(value->endpoint) + 1U;
        memset(value->endpoint + used, 0xa5, sizeof(value->endpoint) - used);
    }
    {
        size_t used = strlen(value->secret_reference) + 1U;
        memset(value->secret_reference + used, 0xa5, sizeof(value->secret_reference) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiWebWorkbenchCloudProfileTransferMalformed(const UmiWebWorkbenchCloudProfile *sample)
{
    (void)sample;
    {
        UmiWebWorkbenchCloudProfile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.profile_id, 'x', sizeof(invalid.profile_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_web_workbench_cloud_profile_validate(&invalid) != UMI_STATUS_OK) ||
            umi_web_workbench_cloud_profile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated profile_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWebWorkbenchCloudProfile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.name, 'x', sizeof(invalid.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_web_workbench_cloud_profile_validate(&invalid) != UMI_STATUS_OK) ||
            umi_web_workbench_cloud_profile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWebWorkbenchCloudProfile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.region, 'x', sizeof(invalid.region));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_web_workbench_cloud_profile_validate(&invalid) != UMI_STATUS_OK) ||
            umi_web_workbench_cloud_profile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated region was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWebWorkbenchCloudProfile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.endpoint, 'x', sizeof(invalid.endpoint));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_web_workbench_cloud_profile_validate(&invalid) != UMI_STATUS_OK) ||
            umi_web_workbench_cloud_profile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated endpoint was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWebWorkbenchCloudProfile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.secret_reference, 'x', sizeof(invalid.secret_reference));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_web_workbench_cloud_profile_validate(&invalid) != UMI_STATUS_OK) ||
            umi_web_workbench_cloud_profile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated secret_reference was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWebWorkbenchCloudProfileTransferCases, UmiWebWorkbenchCloudProfile,
    umi_web_workbench_cloud_profile_archive_encode, umi_web_workbench_cloud_profile_archive_decode,
    UmiWebWorkbenchCloudProfileTransferEqual, UmiWebWorkbenchCloudProfileTransferTails, UmiWebWorkbenchCloudProfileTransferMalformed)

int main(void)
{
    UmiWebWorkbenchCloudProfile profile;
    UmiWebWorkbenchCloudObjectModel *model = calloc(1U, sizeof(*model));
    UmiWebWorkbenchCloudObject object;
    const UmiWebWorkbenchCloudObject *matches[4U];
    assert(model != NULL);
    umi_web_workbench_cloud_profile_init(&profile, "aws-dev", "AWS Dev",
        UMI_WEB_WORKBENCH_CLOUD_AWS);
    assert(umi_web_workbench_copy_text(profile.secret_reference,
        sizeof(profile.secret_reference), "secret://aws/dev") == UMI_STATUS_OK);
    assert(umi_web_workbench_cloud_profile_validate(&profile) == UMI_STATUS_OK);
    if (UmiWebWorkbenchCloudProfileTransferCases(&profile) != 0) return 1;

    umi_web_workbench_cloud_object_model_init(model, &profile);
    memset(&object, 0, sizeof(object));
    assert(umi_web_workbench_copy_text(object.bucket, sizeof(object.bucket),
        "reports") == UMI_STATUS_OK);
    assert(umi_web_workbench_copy_text(object.key, sizeof(object.key),
        "daily/pnl.json") == UMI_STATUS_OK);
    assert(umi_web_workbench_copy_text(object.content_type,
        sizeof(object.content_type), "application/json") == UMI_STATUS_OK);
    object.size_bytes = 1024U;
    assert(umi_web_workbench_cloud_object_upsert(model, &object) == UMI_STATUS_OK);
    assert(umi_web_workbench_cloud_object_query(model, "reports", "pnl",
        matches, 4U) == 1U);
    assert(umi_web_workbench_cloud_object_remove(model, "reports",
        "daily/pnl.json") == UMI_STATUS_OK);
    free(model);
    return 0;
}
