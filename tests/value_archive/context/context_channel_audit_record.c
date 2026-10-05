/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/context_channel_audit_record.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/context_channel/audit_record.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/context_channel/audit_record.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextAuditRecordTransferEqual(const UmiContextAuditRecord *a, const UmiContextAuditRecord *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->audit_id, b->audit_id) == 0 &&
        strcmp(a->actor_id, b->actor_id) == 0 &&
        strcmp(a->action_id, b->action_id) == 0 &&
        strcmp(a->channel_id, b->channel_id) == 0 &&
        strcmp(a->context_id, b->context_id) == 0 &&
        strcmp(a->target_id, b->target_id) == 0 &&
        a->status == b->status &&
        a->timestamp_ms == b->timestamp_ms &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiContextAuditRecordTransferTails(UmiContextAuditRecord *value)
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
    {
        size_t used = strlen(value->channel_id) + 1U;
        memset(value->channel_id + used, 0xa5, sizeof(value->channel_id) - used);
    }
    {
        size_t used = strlen(value->context_id) + 1U;
        memset(value->context_id + used, 0xa5, sizeof(value->context_id) - used);
    }
    {
        size_t used = strlen(value->target_id) + 1U;
        memset(value->target_id + used, 0xa5, sizeof(value->target_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextAuditRecordTransferMalformed(const UmiContextAuditRecord *sample)
{
    (void)sample;
    {
        UmiContextAuditRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.audit_id, 'x', sizeof(invalid.audit_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_audit_record_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_audit_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated audit_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextAuditRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.actor_id, 'x', sizeof(invalid.actor_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_audit_record_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_audit_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated actor_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextAuditRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.action_id, 'x', sizeof(invalid.action_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_audit_record_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_audit_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated action_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextAuditRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.channel_id, 'x', sizeof(invalid.channel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_audit_record_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_audit_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated channel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextAuditRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.context_id, 'x', sizeof(invalid.context_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_audit_record_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_audit_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated context_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextAuditRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target_id, 'x', sizeof(invalid.target_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_audit_record_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_audit_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextAuditRecordTransferCases, UmiContextAuditRecord,
    umi_context_audit_record_archive_encode, umi_context_audit_record_archive_decode,
    UmiContextAuditRecordTransferEqual, UmiContextAuditRecordTransferTails, UmiContextAuditRecordTransferMalformed)

int main(void)
{
    UmiContextAuditRecord value;
    umi_context_audit_record_init(&value);
    value.audit_id[0] = 's';
    value.actor_id[0] = 's';
    value.action_id[0] = 's';
    value.channel_id[0] = 's';
    value.context_id[0] = 's';
    value.target_id[0] = 's';
    value.timestamp_ms = (uint64_t)17U;
    value.revision = (uint64_t)17U;
    if (umi_context_audit_record_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiContextAuditRecordTransferCases(&value) != 0) return 1;

    return 0;
}
