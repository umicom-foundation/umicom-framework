/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_accessibility_node.c
 *
 * PURPOSE:
 *   Verify the context-link accessibility node contract, mutation, validation and stable hashing.
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

#include "umicom/workbench_context_link/accessibility_node.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/workbench_context_link/accessibility_node.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWorkbenchContextLinkAccessibilityNodeTransferEqual(const UmiWorkbenchContextLinkAccessibilityNode *a, const UmiWorkbenchContextLinkAccessibilityNode *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->node_id, b->node_id) == 0 &&
        strcmp(a->role, b->role) == 0 &&
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
static void UmiWorkbenchContextLinkAccessibilityNodeTransferTails(UmiWorkbenchContextLinkAccessibilityNode *value)
{
    (void)value;
    {
        size_t used = strlen(value->node_id) + 1U;
        memset(value->node_id + used, 0xa5, sizeof(value->node_id) - used);
    }
    {
        size_t used = strlen(value->role) + 1U;
        memset(value->role + used, 0xa5, sizeof(value->role) - used);
    }
    {
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiWorkbenchContextLinkAccessibilityNodeTransferMalformed(const UmiWorkbenchContextLinkAccessibilityNode *sample)
{
    (void)sample;
    {
        UmiWorkbenchContextLinkAccessibilityNode invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.node_id, 'x', sizeof(invalid.node_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_accessibility_node_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_accessibility_node_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated node_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkAccessibilityNode invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.role, 'x', sizeof(invalid.role));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_accessibility_node_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_accessibility_node_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated role was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkAccessibilityNode invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.label, 'x', sizeof(invalid.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_accessibility_node_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_accessibility_node_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated label was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWorkbenchContextLinkAccessibilityNodeTransferCases, UmiWorkbenchContextLinkAccessibilityNode,
    umi_workbench_context_link_accessibility_node_archive_encode, umi_workbench_context_link_accessibility_node_archive_decode,
    UmiWorkbenchContextLinkAccessibilityNodeTransferEqual, UmiWorkbenchContextLinkAccessibilityNodeTransferTails, UmiWorkbenchContextLinkAccessibilityNodeTransferMalformed)

int main(void)
{
    UmiWorkbenchContextLinkAccessibilityNode record;
    UmiWorkbenchContextLinkAccessibilityNode copy;
    uint64_t first_hash;
    umi_workbench_context_link_accessibility_node_init(&record, "accessibility_node-id");
    assert(umi_workbench_context_link_accessibility_node_validate(&record) == UMI_STATUS_OK);
    if (UmiWorkbenchContextLinkAccessibilityNodeTransferCases(&record) != 0) return 1;

    assert(umi_workbench_context_link_accessibility_node_set_primary(&record, "primary") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_accessibility_node_set_secondary(&record, "secondary") == UMI_STATUS_OK);
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.colour = UMI_CONTEXT_COLOUR_BLUE;
    record.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    record.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE;
    first_hash = umi_workbench_context_link_accessibility_node_hash(&record);
    assert(first_hash != 0U);
    assert(umi_workbench_context_link_accessibility_node_copy(&copy, &record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_accessibility_node_hash(&copy) == first_hash);
    umi_workbench_context_link_accessibility_node_touch(&copy, 9U, 1000U);
    assert(copy.sequence == 9U);
    assert(copy.timestamp_ms == 1000U);
    assert(copy.revision > record.revision);
    assert(strcmp(copy.node_id, record.node_id) == 0);
    return 0;
}
