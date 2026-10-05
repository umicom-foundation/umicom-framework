/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_data_session.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the data session enterprise data capability.
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
#include "umicom/data/enterprise/data_session.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/data/enterprise/data_session.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDataSessionTransferEqual(const UmiDataSession *a, const UmiDataSession *b)
{
    return strcmp(a->session_id, b->session_id) == 0 &&
        strcmp(a->principal_id, b->principal_id) == 0 &&
        a->consistency == b->consistency &&
        a->started_at == b->started_at &&
        a->last_activity == b->last_activity &&
        a->transaction_open == b->transaction_open;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDataSessionTransferTails(UmiDataSession *value)
{
    (void)value;
    {
        size_t used = strlen(value->session_id) + 1U;
        memset(value->session_id + used, 0xa5, sizeof(value->session_id) - used);
    }
    {
        size_t used = strlen(value->principal_id) + 1U;
        memset(value->principal_id + used, 0xa5, sizeof(value->principal_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDataSessionTransferMalformed(const UmiDataSession *sample)
{
    (void)sample;
    {
        UmiDataSession invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.session_id, 'x', sizeof(invalid.session_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_data_session_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_data_session_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated session_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataSession invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.principal_id, 'x', sizeof(invalid.principal_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_data_session_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_data_session_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated principal_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDataSessionTransferCases, UmiDataSession,
    umi_data_data_session_archive_encode, umi_data_data_session_archive_decode,
    UmiDataSessionTransferEqual, UmiDataSessionTransferTails, UmiDataSessionTransferMalformed)

int main(void) {
    UmiDataSession item;
    CHECK(umi_data_data_session_init(&item,"s1","user",UMI_DATA_CONSISTENCY_SESSION,10U) == UMI_STATUS_OK);
    if (UmiDataSessionTransferCases(&item) != 0) return 1;

    CHECK(item.last_activity==10U);
    return 0;
}
