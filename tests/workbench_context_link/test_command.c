/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_command.c
 *
 * PURPOSE:
 *   Verify the context-link command record contract, mutation, validation and stable hashing.
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

#include "umicom/workbench_context_link/command.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/workbench_context_link/command.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWorkbenchContextLinkCommandTransferEqual(const UmiWorkbenchContextLinkCommand *a, const UmiWorkbenchContextLinkCommand *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->command_id, b->command_id) == 0 &&
        strcmp(a->group_id, b->group_id) == 0 &&
        strcmp(a->panel_id, b->panel_id) == 0 &&
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
static void UmiWorkbenchContextLinkCommandTransferTails(UmiWorkbenchContextLinkCommand *value)
{
    (void)value;
    {
        size_t used = strlen(value->command_id) + 1U;
        memset(value->command_id + used, 0xa5, sizeof(value->command_id) - used);
    }
    {
        size_t used = strlen(value->group_id) + 1U;
        memset(value->group_id + used, 0xa5, sizeof(value->group_id) - used);
    }
    {
        size_t used = strlen(value->panel_id) + 1U;
        memset(value->panel_id + used, 0xa5, sizeof(value->panel_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiWorkbenchContextLinkCommandTransferMalformed(const UmiWorkbenchContextLinkCommand *sample)
{
    (void)sample;
    {
        UmiWorkbenchContextLinkCommand invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.command_id, 'x', sizeof(invalid.command_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_command_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_command_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated command_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkCommand invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.group_id, 'x', sizeof(invalid.group_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_command_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_command_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated group_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkCommand invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.panel_id, 'x', sizeof(invalid.panel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_command_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_command_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated panel_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWorkbenchContextLinkCommandTransferCases, UmiWorkbenchContextLinkCommand,
    umi_workbench_context_link_command_archive_encode, umi_workbench_context_link_command_archive_decode,
    UmiWorkbenchContextLinkCommandTransferEqual, UmiWorkbenchContextLinkCommandTransferTails, UmiWorkbenchContextLinkCommandTransferMalformed)

/* Exercise the caller-workspace API using this owner\'s real fixture and field comparator. */
#include "../value_archive/workspace_cases.h"
UMI_TEST_VALUE_WORKSPACE(UmiWorkbenchContextLinkCommandTransferCasesWithWorkspace, UmiWorkbenchContextLinkCommand,
    umi_workbench_context_link_command_archive_encode, umi_workbench_context_link_command_archive_decode_with_workspace, UmiWorkbenchContextLinkCommandTransferEqual)

#include "../value_archive/conditional_cases.h"
UMI_TEST_CONDITIONAL_ARCHIVE(UmiWorkbenchContextLinkCommandConditionalArchiveCases, UmiWorkbenchContextLinkCommand,
    umi_workbench_context_link_command_archive_encode, umi_workbench_context_link_command_archive_decode_if_unchanged,
    UmiWorkbenchContextLinkCommandTransferEqual, UmiWorkbenchContextLinkCommandTransferTails, UmiWorkbenchContextLinkCommandTransferMalformed)


/* Delayed context edits must preserve newer work and reject exhausted tokens. */
#include "../record_integrity/reviewed_value_cases.h"
UMI_TEST_REVIEWED_VALUE(UmiWorkbenchContextLinkCommandReviewedCases, UmiWorkbenchContextLinkCommand,
    command_id, panel_id, umi_workbench_context_link_command_replace_if_current, UmiWorkbenchContextLinkCommandTransferEqual)
static int UmiWorkbenchContextLinkCommandMutationGuards(const UmiWorkbenchContextLinkCommand *sample)
{
    UMI_TEST_REVIEWED_TEXT_FIELD(UmiWorkbenchContextLinkCommand, group_id, umi_workbench_context_link_command_set_primary, sample);
    UMI_TEST_REVIEWED_TEXT_FIELD(UmiWorkbenchContextLinkCommand, panel_id, umi_workbench_context_link_command_set_secondary, sample);
    UmiWorkbenchContextLinkCommand value = *sample;
    unsigned char before[sizeof(value)];
    value.revision = UINT64_MAX - 1U;
    umi_workbench_context_link_command_touch(&value, 33U, 44U);
    if (value.revision != UINT64_MAX || value.sequence != 33U ||
        value.timestamp_ms != 44U) return 1;
    memcpy(before, &value, sizeof(value));
    umi_workbench_context_link_command_touch(&value, 55U, 66U);
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    return 0;
}

int main(void)
{
    UmiWorkbenchContextLinkCommand record;
    UmiWorkbenchContextLinkCommand copy;
    uint64_t first_hash;
    umi_workbench_context_link_command_init(&record, "command-id");
    assert(umi_workbench_context_link_command_validate(&record) == UMI_STATUS_OK);
    if (UmiWorkbenchContextLinkCommandTransferCases(&record) != 0) return 1;
    if (UmiWorkbenchContextLinkCommandTransferCasesWithWorkspace(&record) != 0) return 1;
    if (UmiWorkbenchContextLinkCommandConditionalArchiveCases(&record) != 0) return 1;
    if (UmiWorkbenchContextLinkCommandReviewedCases(&record) != 0) return 1;
    if (UmiWorkbenchContextLinkCommandMutationGuards(&record) != 0) return 1;

    assert(umi_workbench_context_link_command_set_primary(&record, "primary") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_command_set_secondary(&record, "secondary") == UMI_STATUS_OK);
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.colour = UMI_CONTEXT_COLOUR_BLUE;
    record.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    record.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE;
    first_hash = umi_workbench_context_link_command_hash(&record);
    assert(first_hash != 0U);
    assert(umi_workbench_context_link_command_copy(&copy, &record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_command_hash(&copy) == first_hash);
    umi_workbench_context_link_command_touch(&copy, 9U, 1000U);
    assert(copy.sequence == 9U);
    assert(copy.timestamp_ms == 1000U);
    assert(copy.revision > record.revision);
    assert(strcmp(copy.command_id, record.command_id) == 0);
    return 0;
}
