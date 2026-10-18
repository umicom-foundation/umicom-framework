/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_media_bridge.c
 *
 * PURPOSE:
 *   Verify the media context bridge contract, mutation, validation and stable hashing.
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

#include "umicom/workbench_context_link/media_bridge.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/workbench_context_link/media_bridge.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWorkbenchContextLinkMediaBridgeTransferEqual(const UmiWorkbenchContextLinkMediaBridge *a, const UmiWorkbenchContextLinkMediaBridge *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->bridge_id, b->bridge_id) == 0 &&
        strcmp(a->panel_id, b->panel_id) == 0 &&
        strcmp(a->group_id, b->group_id) == 0 &&
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
static void UmiWorkbenchContextLinkMediaBridgeTransferTails(UmiWorkbenchContextLinkMediaBridge *value)
{
    (void)value;
    {
        size_t used = strlen(value->bridge_id) + 1U;
        memset(value->bridge_id + used, 0xa5, sizeof(value->bridge_id) - used);
    }
    {
        size_t used = strlen(value->panel_id) + 1U;
        memset(value->panel_id + used, 0xa5, sizeof(value->panel_id) - used);
    }
    {
        size_t used = strlen(value->group_id) + 1U;
        memset(value->group_id + used, 0xa5, sizeof(value->group_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiWorkbenchContextLinkMediaBridgeTransferMalformed(const UmiWorkbenchContextLinkMediaBridge *sample)
{
    (void)sample;
    {
        UmiWorkbenchContextLinkMediaBridge invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.bridge_id, 'x', sizeof(invalid.bridge_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_media_bridge_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_media_bridge_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated bridge_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkMediaBridge invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.panel_id, 'x', sizeof(invalid.panel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_media_bridge_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_media_bridge_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated panel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkMediaBridge invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.group_id, 'x', sizeof(invalid.group_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_media_bridge_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_media_bridge_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated group_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWorkbenchContextLinkMediaBridgeTransferCases, UmiWorkbenchContextLinkMediaBridge,
    umi_workbench_context_link_media_bridge_archive_encode, umi_workbench_context_link_media_bridge_archive_decode,
    UmiWorkbenchContextLinkMediaBridgeTransferEqual, UmiWorkbenchContextLinkMediaBridgeTransferTails, UmiWorkbenchContextLinkMediaBridgeTransferMalformed)

/* Exercise the caller-workspace API using this owner\'s real fixture and field comparator. */
#include "../value_archive/workspace_cases.h"
UMI_TEST_VALUE_WORKSPACE(UmiWorkbenchContextLinkMediaBridgeTransferCasesWithWorkspace, UmiWorkbenchContextLinkMediaBridge,
    umi_workbench_context_link_media_bridge_archive_encode, umi_workbench_context_link_media_bridge_archive_decode_with_workspace, UmiWorkbenchContextLinkMediaBridgeTransferEqual)

#include "../value_archive/conditional_cases.h"
UMI_TEST_CONDITIONAL_ARCHIVE(UmiWorkbenchContextLinkMediaBridgeConditionalArchiveCases, UmiWorkbenchContextLinkMediaBridge,
    umi_workbench_context_link_media_bridge_archive_encode, umi_workbench_context_link_media_bridge_archive_decode_if_unchanged,
    UmiWorkbenchContextLinkMediaBridgeTransferEqual, UmiWorkbenchContextLinkMediaBridgeTransferTails, UmiWorkbenchContextLinkMediaBridgeTransferMalformed)


/* Delayed context edits must preserve newer work and reject exhausted tokens. */
#include "../record_integrity/reviewed_value_cases.h"
UMI_TEST_REVIEWED_VALUE(UmiWorkbenchContextLinkMediaBridgeReviewedCases, UmiWorkbenchContextLinkMediaBridge,
    bridge_id, group_id, umi_workbench_context_link_media_bridge_replace_if_current, UmiWorkbenchContextLinkMediaBridgeTransferEqual)
static int UmiWorkbenchContextLinkMediaBridgeMutationGuards(const UmiWorkbenchContextLinkMediaBridge *sample)
{
    UMI_TEST_REVIEWED_TEXT_FIELD(UmiWorkbenchContextLinkMediaBridge, panel_id, umi_workbench_context_link_media_bridge_set_primary, sample);
    UMI_TEST_REVIEWED_TEXT_FIELD(UmiWorkbenchContextLinkMediaBridge, group_id, umi_workbench_context_link_media_bridge_set_secondary, sample);
    UmiWorkbenchContextLinkMediaBridge value = *sample;
    unsigned char before[sizeof(value)];
    value.revision = UINT64_MAX - 1U;
    umi_workbench_context_link_media_bridge_touch(&value, 33U, 44U);
    if (value.revision != UINT64_MAX || value.sequence != 33U ||
        value.timestamp_ms != 44U) return 1;
    memcpy(before, &value, sizeof(value));
    umi_workbench_context_link_media_bridge_touch(&value, 55U, 66U);
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    return 0;
}

int main(void)
{
    UmiWorkbenchContextLinkMediaBridge record;
    UmiWorkbenchContextLinkMediaBridge copy;
    uint64_t first_hash;
    umi_workbench_context_link_media_bridge_init(&record, "media_bridge-id");
    assert(umi_workbench_context_link_media_bridge_validate(&record) == UMI_STATUS_OK);
    if (UmiWorkbenchContextLinkMediaBridgeTransferCases(&record) != 0) return 1;
    if (UmiWorkbenchContextLinkMediaBridgeTransferCasesWithWorkspace(&record) != 0) return 1;
    if (UmiWorkbenchContextLinkMediaBridgeConditionalArchiveCases(&record) != 0) return 1;
    if (UmiWorkbenchContextLinkMediaBridgeReviewedCases(&record) != 0) return 1;
    if (UmiWorkbenchContextLinkMediaBridgeMutationGuards(&record) != 0) return 1;

    assert(umi_workbench_context_link_media_bridge_set_primary(&record, "primary") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_media_bridge_set_secondary(&record, "secondary") == UMI_STATUS_OK);
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.colour = UMI_CONTEXT_COLOUR_BLUE;
    record.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    record.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE;
    first_hash = umi_workbench_context_link_media_bridge_hash(&record);
    assert(first_hash != 0U);
    assert(umi_workbench_context_link_media_bridge_copy(&copy, &record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_media_bridge_hash(&copy) == first_hash);
    umi_workbench_context_link_media_bridge_touch(&copy, 9U, 1000U);
    assert(copy.sequence == 9U);
    assert(copy.timestamp_ms == 1000U);
    assert(copy.revision > record.revision);
    assert(strcmp(copy.bridge_id, record.bridge_id) == 0);
    return 0;
}
