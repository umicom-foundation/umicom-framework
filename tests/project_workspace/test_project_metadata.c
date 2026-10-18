/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_project_metadata.c
 *
 * PURPOSE:
 *   Implement the test project metadata behavior for
 *   Umicom Framework.
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
#include "umicom/project/workspace/project_metadata.h"
#include "../value_archive/construction_cases.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/project/workspace/project_metadata.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiProjectWorkspaceProjectMetadataTransferEqual(const UmiProjectWorkspaceProjectMetadata *a, const UmiProjectWorkspaceProjectMetadata *b)
{
    return a->base.structure_size == b->base.structure_size &&
        a->base.api_version == b->base.api_version &&
        strcmp(a->base.id, b->base.id) == 0 &&
        strcmp(a->base.name, b->base.name) == 0 &&
        strcmp(a->base.detail, b->base.detail) == 0 &&
        a->base.revision == b->base.revision &&
        a->base.flags == b->base.flags &&
        a->base.priority == b->base.priority &&
        a->base.state == b->base.state &&
        a->base.enabled == b->base.enabled &&
        a->metric == b->metric;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiProjectWorkspaceProjectMetadataTransferTails(UmiProjectWorkspaceProjectMetadata *value)
{
    (void)value;
    {
        size_t used = strlen(value->base.id) + 1U;
        memset(value->base.id + used, 0xa5, sizeof(value->base.id) - used);
    }
    {
        size_t used = strlen(value->base.name) + 1U;
        memset(value->base.name + used, 0xa5, sizeof(value->base.name) - used);
    }
    {
        size_t used = strlen(value->base.detail) + 1U;
        memset(value->base.detail + used, 0xa5, sizeof(value->base.detail) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiProjectWorkspaceProjectMetadataTransferMalformed(const UmiProjectWorkspaceProjectMetadata *sample)
{
    (void)sample;
    {
        UmiProjectWorkspaceProjectMetadata invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.base.id, 'x', sizeof(invalid.base.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_project_workspace_project_metadata_validate(&invalid) != UMI_STATUS_OK) ||
            umi_project_workspace_project_metadata_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated base.id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiProjectWorkspaceProjectMetadata invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.base.name, 'x', sizeof(invalid.base.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_project_workspace_project_metadata_validate(&invalid) != UMI_STATUS_OK) ||
            umi_project_workspace_project_metadata_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated base.name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiProjectWorkspaceProjectMetadata invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.base.detail, 'x', sizeof(invalid.base.detail));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_project_workspace_project_metadata_validate(&invalid) != UMI_STATUS_OK) ||
            umi_project_workspace_project_metadata_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated base.detail was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiProjectWorkspaceProjectMetadataTransferCases, UmiProjectWorkspaceProjectMetadata,
    umi_project_workspace_project_metadata_archive_encode, umi_project_workspace_project_metadata_archive_decode,
    UmiProjectWorkspaceProjectMetadataTransferEqual, UmiProjectWorkspaceProjectMetadataTransferTails, UmiProjectWorkspaceProjectMetadataTransferMalformed)

/* Exercise the caller-workspace API using this owner\'s real fixture and field comparator. */
#include "../value_archive/workspace_cases.h"
UMI_TEST_VALUE_WORKSPACE(UmiProjectWorkspaceProjectMetadataTransferCasesWithWorkspace, UmiProjectWorkspaceProjectMetadata,
    umi_project_workspace_project_metadata_archive_encode, umi_project_workspace_project_metadata_archive_decode_with_workspace, UmiProjectWorkspaceProjectMetadataTransferEqual)

#include "../value_archive/conditional_cases.h"
UMI_TEST_CONDITIONAL_ARCHIVE(UmiProjectWorkspaceProjectMetadataConditionalArchiveCases, UmiProjectWorkspaceProjectMetadata,
    umi_project_workspace_project_metadata_archive_encode, umi_project_workspace_project_metadata_archive_decode_if_unchanged,
    UmiProjectWorkspaceProjectMetadataTransferEqual, UmiProjectWorkspaceProjectMetadataTransferTails, UmiProjectWorkspaceProjectMetadataTransferMalformed)



/* A rejected replacement must preserve the previously accepted project metadata value. */
static int UmiProjectWorkspaceProjectMetadataConstructionCases(void)
{
    UmiProjectWorkspaceProjectMetadata *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_project_workspace_project_metadata_init(value, "item") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->base.id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_project_workspace_project_metadata_init(value, oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_project_workspace_project_metadata_init(value, value->base.id) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->base.id, "item") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiProjectWorkspaceProjectMetadataConstructionCases() != 0) return 1;

    UmiProjectWorkspaceProjectMetadata a,b;
    CHECK(umi_project_workspace_project_metadata_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_metadata_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_metadata_set_name(&a,"Project Metadata")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_metadata_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_project_metadata_set_metric(&a,7U);
    CHECK(umi_project_workspace_project_metadata_validate(&a)==UMI_STATUS_OK);
    if (UmiProjectWorkspaceProjectMetadataTransferCases(&a) != 0) return 1;
    if (UmiProjectWorkspaceProjectMetadataTransferCasesWithWorkspace(&a) != 0) return 1;
    if (UmiProjectWorkspaceProjectMetadataConditionalArchiveCases(&a) != 0) return 1;

    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_project_metadata_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Project Metadata")==0);
    return 0;
}
