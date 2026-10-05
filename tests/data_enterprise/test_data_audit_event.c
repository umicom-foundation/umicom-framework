/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_data_audit_event.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the data audit event enterprise data capability.
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
#include "umicom/data/enterprise/data_audit_event.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/data/enterprise/data_audit_event.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDataAuditEventTransferEqual(const UmiDataAuditEvent *a, const UmiDataAuditEvent *b)
{
    return strcmp(a->event_id, b->event_id) == 0 &&
        strcmp(a->operation_id, b->operation_id) == 0 &&
        strcmp(a->principal_id, b->principal_id) == 0 &&
        strcmp(a->action, b->action) == 0 &&
        a->timestamp == b->timestamp &&
        a->outcome == b->outcome;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDataAuditEventTransferTails(UmiDataAuditEvent *value)
{
    (void)value;
    {
        size_t used = strlen(value->event_id) + 1U;
        memset(value->event_id + used, 0xa5, sizeof(value->event_id) - used);
    }
    {
        size_t used = strlen(value->operation_id) + 1U;
        memset(value->operation_id + used, 0xa5, sizeof(value->operation_id) - used);
    }
    {
        size_t used = strlen(value->principal_id) + 1U;
        memset(value->principal_id + used, 0xa5, sizeof(value->principal_id) - used);
    }
    {
        size_t used = strlen(value->action) + 1U;
        memset(value->action + used, 0xa5, sizeof(value->action) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDataAuditEventTransferMalformed(const UmiDataAuditEvent *sample)
{
    (void)sample;
    {
        UmiDataAuditEvent invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.event_id, 'x', sizeof(invalid.event_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_data_audit_event_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_data_audit_event_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated event_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataAuditEvent invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.operation_id, 'x', sizeof(invalid.operation_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_data_audit_event_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_data_audit_event_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated operation_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataAuditEvent invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.principal_id, 'x', sizeof(invalid.principal_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_data_audit_event_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_data_audit_event_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated principal_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataAuditEvent invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.action, 'x', sizeof(invalid.action));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_data_audit_event_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_data_audit_event_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated action was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDataAuditEventTransferCases, UmiDataAuditEvent,
    umi_data_data_audit_event_archive_encode, umi_data_data_audit_event_archive_decode,
    UmiDataAuditEventTransferEqual, UmiDataAuditEventTransferTails, UmiDataAuditEventTransferMalformed)

int main(void) {
    UmiDataAuditEvent item;
    CHECK(umi_data_data_audit_event_init(&item,"e1","op1","user","query",10U,UMI_STATUS_OK) == UMI_STATUS_OK);
    if (UmiDataAuditEventTransferCases(&item) != 0) return 1;

    CHECK(item.outcome==UMI_STATUS_OK);
    return 0;
}
