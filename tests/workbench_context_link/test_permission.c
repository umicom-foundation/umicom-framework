/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_permission.c
 *
 * PURPOSE:
 *   Verify the context-link permission record contract, mutation, validation and stable hashing.
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
#include <assert.h>
#include <string.h>

#include "umicom/workbench_context_link/permission.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/workbench_context_link/permission.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWorkbenchContextLinkPermissionTransferEqual(const UmiWorkbenchContextLinkPermission *a, const UmiWorkbenchContextLinkPermission *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->permission_id, b->permission_id) == 0 &&
        strcmp(a->subject_id, b->subject_id) == 0 &&
        strcmp(a->action_id, b->action_id) == 0 &&
        a->context_kind == b->context_kind &&
        a->colour == b->colour &&
        a->mode == b->mode &&
        a->state == b->state &&
        a->origin == b->origin &&
        a->priority == b->priority &&
        a->flags == b->flags &&
        a->sequence == b->sequence &&
        a->timestamp_ms == b->timestamp_ms &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiWorkbenchContextLinkPermissionTransferTails(UmiWorkbenchContextLinkPermission *value)
{
    (void)value;
    {
        size_t used = strlen(value->permission_id) + 1U;
        memset(value->permission_id + used, 0xa5, sizeof(value->permission_id) - used);
    }
    {
        size_t used = strlen(value->subject_id) + 1U;
        memset(value->subject_id + used, 0xa5, sizeof(value->subject_id) - used);
    }
    {
        size_t used = strlen(value->action_id) + 1U;
        memset(value->action_id + used, 0xa5, sizeof(value->action_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiWorkbenchContextLinkPermissionTransferMalformed(const UmiWorkbenchContextLinkPermission *sample)
{
    (void)sample;
    {
        UmiWorkbenchContextLinkPermission invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.permission_id, 'x', sizeof(invalid.permission_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_permission_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_permission_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated permission_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkPermission invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.subject_id, 'x', sizeof(invalid.subject_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_permission_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_permission_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated subject_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkPermission invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.action_id, 'x', sizeof(invalid.action_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_permission_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_permission_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated action_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWorkbenchContextLinkPermissionTransferCases, UmiWorkbenchContextLinkPermission,
    umi_workbench_context_link_permission_archive_encode, umi_workbench_context_link_permission_archive_decode,
    UmiWorkbenchContextLinkPermissionTransferEqual, UmiWorkbenchContextLinkPermissionTransferTails, UmiWorkbenchContextLinkPermissionTransferMalformed)

int main(void)
{
    UmiWorkbenchContextLinkPermission record;
    UmiWorkbenchContextLinkPermission copy;
    uint64_t first_hash;
    umi_workbench_context_link_permission_init(&record, "permission-id");
    assert(umi_workbench_context_link_permission_validate(&record) == UMI_STATUS_OK);
    if (UmiWorkbenchContextLinkPermissionTransferCases(&record) != 0) return 1;

    assert(umi_workbench_context_link_permission_set_primary(&record, "primary") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_permission_set_secondary(&record, "secondary") == UMI_STATUS_OK);
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.colour = UMI_CONTEXT_COLOUR_BLUE;
    record.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    record.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE;
    first_hash = umi_workbench_context_link_permission_hash(&record);
    assert(first_hash != 0U);
    assert(umi_workbench_context_link_permission_copy(&copy, &record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_permission_hash(&copy) == first_hash);
    umi_workbench_context_link_permission_touch(&copy, 9U, 1000U);
    assert(copy.sequence == 9U);
    assert(copy.timestamp_ms == 1000U);
    assert(copy.revision > record.revision);
    assert(strcmp(copy.permission_id, record.permission_id) == 0);
    return 0;
}
