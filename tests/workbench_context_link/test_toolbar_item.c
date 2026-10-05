/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_toolbar_item.c
 *
 * PURPOSE:
 *   Verify the context-link toolbar item contract, mutation, validation and stable hashing.
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

#include "umicom/workbench_context_link/toolbar_item.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/workbench_context_link/toolbar_item.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWorkbenchContextLinkToolbarItemTransferEqual(const UmiWorkbenchContextLinkToolbarItem *a, const UmiWorkbenchContextLinkToolbarItem *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->item_id, b->item_id) == 0 &&
        strcmp(a->command_id, b->command_id) == 0 &&
        strcmp(a->label, b->label) == 0 &&
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
static void UmiWorkbenchContextLinkToolbarItemTransferTails(UmiWorkbenchContextLinkToolbarItem *value)
{
    (void)value;
    {
        size_t used = strlen(value->item_id) + 1U;
        memset(value->item_id + used, 0xa5, sizeof(value->item_id) - used);
    }
    {
        size_t used = strlen(value->command_id) + 1U;
        memset(value->command_id + used, 0xa5, sizeof(value->command_id) - used);
    }
    {
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiWorkbenchContextLinkToolbarItemTransferMalformed(const UmiWorkbenchContextLinkToolbarItem *sample)
{
    (void)sample;
    {
        UmiWorkbenchContextLinkToolbarItem invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.item_id, 'x', sizeof(invalid.item_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_toolbar_item_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_toolbar_item_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated item_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkToolbarItem invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.command_id, 'x', sizeof(invalid.command_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_toolbar_item_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_toolbar_item_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated command_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkToolbarItem invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.label, 'x', sizeof(invalid.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_toolbar_item_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_toolbar_item_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated label was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWorkbenchContextLinkToolbarItemTransferCases, UmiWorkbenchContextLinkToolbarItem,
    umi_workbench_context_link_toolbar_item_archive_encode, umi_workbench_context_link_toolbar_item_archive_decode,
    UmiWorkbenchContextLinkToolbarItemTransferEqual, UmiWorkbenchContextLinkToolbarItemTransferTails, UmiWorkbenchContextLinkToolbarItemTransferMalformed)

int main(void)
{
    UmiWorkbenchContextLinkToolbarItem record;
    UmiWorkbenchContextLinkToolbarItem copy;
    uint64_t first_hash;
    umi_workbench_context_link_toolbar_item_init(&record, "toolbar_item-id");
    assert(umi_workbench_context_link_toolbar_item_validate(&record) == UMI_STATUS_OK);
    if (UmiWorkbenchContextLinkToolbarItemTransferCases(&record) != 0) return 1;

    assert(umi_workbench_context_link_toolbar_item_set_primary(&record, "primary") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_toolbar_item_set_secondary(&record, "secondary") == UMI_STATUS_OK);
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.colour = UMI_CONTEXT_COLOUR_BLUE;
    record.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    record.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE;
    first_hash = umi_workbench_context_link_toolbar_item_hash(&record);
    assert(first_hash != 0U);
    assert(umi_workbench_context_link_toolbar_item_copy(&copy, &record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_toolbar_item_hash(&copy) == first_hash);
    umi_workbench_context_link_toolbar_item_touch(&copy, 9U, 1000U);
    assert(copy.sequence == 9U);
    assert(copy.timestamp_ms == 1000U);
    assert(copy.revision > record.revision);
    assert(strcmp(copy.item_id, record.item_id) == 0);
    return 0;
}
