/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_audit_record.c
 *
 * PURPOSE:
 *   Verify the context-link audit record contract, mutation, validation and stable hashing.
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

#include "umicom/workbench_context_link/audit_record.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/workbench_context_link/audit_record.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWorkbenchContextLinkAuditRecordTransferEqual(const UmiWorkbenchContextLinkAuditRecord *a, const UmiWorkbenchContextLinkAuditRecord *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->audit_id, b->audit_id) == 0 &&
        strcmp(a->actor_id, b->actor_id) == 0 &&
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
static void UmiWorkbenchContextLinkAuditRecordTransferTails(UmiWorkbenchContextLinkAuditRecord *value)
{
    (void)value;
    {
        size_t used = strlen(value->audit_id) + 1U;
        memset(value->audit_id + used, 0xa5, sizeof(value->audit_id) - used);
    }
    {
        size_t used = strlen(value->actor_id) + 1U;
        memset(value->actor_id + used, 0xa5, sizeof(value->actor_id) - used);
    }
    {
        size_t used = strlen(value->action_id) + 1U;
        memset(value->action_id + used, 0xa5, sizeof(value->action_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiWorkbenchContextLinkAuditRecordTransferMalformed(const UmiWorkbenchContextLinkAuditRecord *sample)
{
    (void)sample;
    {
        UmiWorkbenchContextLinkAuditRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.audit_id, 'x', sizeof(invalid.audit_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_audit_record_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_audit_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated audit_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkAuditRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.actor_id, 'x', sizeof(invalid.actor_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_audit_record_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_audit_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated actor_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkAuditRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.action_id, 'x', sizeof(invalid.action_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_audit_record_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_audit_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated action_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWorkbenchContextLinkAuditRecordTransferCases, UmiWorkbenchContextLinkAuditRecord,
    umi_workbench_context_link_audit_record_archive_encode, umi_workbench_context_link_audit_record_archive_decode,
    UmiWorkbenchContextLinkAuditRecordTransferEqual, UmiWorkbenchContextLinkAuditRecordTransferTails, UmiWorkbenchContextLinkAuditRecordTransferMalformed)

int main(void)
{
    UmiWorkbenchContextLinkAuditRecord record;
    UmiWorkbenchContextLinkAuditRecord copy;
    uint64_t first_hash;
    umi_workbench_context_link_audit_record_init(&record, "audit_record-id");
    assert(umi_workbench_context_link_audit_record_validate(&record) == UMI_STATUS_OK);
    if (UmiWorkbenchContextLinkAuditRecordTransferCases(&record) != 0) return 1;

    assert(umi_workbench_context_link_audit_record_set_primary(&record, "primary") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_audit_record_set_secondary(&record, "secondary") == UMI_STATUS_OK);
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.colour = UMI_CONTEXT_COLOUR_BLUE;
    record.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    record.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE;
    first_hash = umi_workbench_context_link_audit_record_hash(&record);
    assert(first_hash != 0U);
    assert(umi_workbench_context_link_audit_record_copy(&copy, &record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_audit_record_hash(&copy) == first_hash);
    umi_workbench_context_link_audit_record_touch(&copy, 9U, 1000U);
    assert(copy.sequence == 9U);
    assert(copy.timestamp_ms == 1000U);
    assert(copy.revision > record.revision);
    assert(strcmp(copy.audit_id, record.audit_id) == 0);
    return 0;
}
