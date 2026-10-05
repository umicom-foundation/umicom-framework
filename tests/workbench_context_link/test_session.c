/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_session.c
 *
 * PURPOSE:
 *   Verify the context-link workbench session contract, mutation, validation and stable hashing.
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

#include "umicom/workbench_context_link/session.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/workbench_context_link/session.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWorkbenchContextLinkSessionTransferEqual(const UmiWorkbenchContextLinkSession *a, const UmiWorkbenchContextLinkSession *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->session_id, b->session_id) == 0 &&
        strcmp(a->workspace_id, b->workspace_id) == 0 &&
        strcmp(a->layout_id, b->layout_id) == 0 &&
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
static void UmiWorkbenchContextLinkSessionTransferTails(UmiWorkbenchContextLinkSession *value)
{
    (void)value;
    {
        size_t used = strlen(value->session_id) + 1U;
        memset(value->session_id + used, 0xa5, sizeof(value->session_id) - used);
    }
    {
        size_t used = strlen(value->workspace_id) + 1U;
        memset(value->workspace_id + used, 0xa5, sizeof(value->workspace_id) - used);
    }
    {
        size_t used = strlen(value->layout_id) + 1U;
        memset(value->layout_id + used, 0xa5, sizeof(value->layout_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiWorkbenchContextLinkSessionTransferMalformed(const UmiWorkbenchContextLinkSession *sample)
{
    (void)sample;
    {
        UmiWorkbenchContextLinkSession invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.session_id, 'x', sizeof(invalid.session_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_session_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_session_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated session_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkSession invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.workspace_id, 'x', sizeof(invalid.workspace_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_session_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_session_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated workspace_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkSession invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.layout_id, 'x', sizeof(invalid.layout_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_session_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_session_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated layout_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWorkbenchContextLinkSessionTransferCases, UmiWorkbenchContextLinkSession,
    umi_workbench_context_link_session_archive_encode, umi_workbench_context_link_session_archive_decode,
    UmiWorkbenchContextLinkSessionTransferEqual, UmiWorkbenchContextLinkSessionTransferTails, UmiWorkbenchContextLinkSessionTransferMalformed)

int main(void)
{
    UmiWorkbenchContextLinkSession record;
    UmiWorkbenchContextLinkSession copy;
    uint64_t first_hash;
    umi_workbench_context_link_session_init(&record, "session-id");
    assert(umi_workbench_context_link_session_validate(&record) == UMI_STATUS_OK);
    if (UmiWorkbenchContextLinkSessionTransferCases(&record) != 0) return 1;

    assert(umi_workbench_context_link_session_set_primary(&record, "primary") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_session_set_secondary(&record, "secondary") == UMI_STATUS_OK);
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.colour = UMI_CONTEXT_COLOUR_BLUE;
    record.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    record.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE;
    first_hash = umi_workbench_context_link_session_hash(&record);
    assert(first_hash != 0U);
    assert(umi_workbench_context_link_session_copy(&copy, &record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_session_hash(&copy) == first_hash);
    umi_workbench_context_link_session_touch(&copy, 9U, 1000U);
    assert(copy.sequence == 9U);
    assert(copy.timestamp_ms == 1000U);
    assert(copy.revision > record.revision);
    assert(strcmp(copy.session_id, record.session_id) == 0);
    return 0;
}
