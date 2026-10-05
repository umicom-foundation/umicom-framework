/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_fix_venue_contracts.c
 *
 * PURPOSE:
 *   Validate fix venue contracts behaviour in the trading foundation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This focused regression test uses deterministic values so changes to the trading contract are visible immediately.
 */

/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include "umicom/trading/trading.h"
/*
 * Exercise route and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus route(void *instance,const UmiOrderRequest *request){return instance!=NULL&&request!=NULL?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;}
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/trading/fix_boundary.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFixSessionInfoTransferEqual(const UmiFixSessionInfo *a, const UmiFixSessionInfo *b)
{
    return strcmp(a->sender_comp_id, b->sender_comp_id) == 0 &&
        strcmp(a->target_comp_id, b->target_comp_id) == 0 &&
        a->next_out_sequence == b->next_out_sequence &&
        a->next_in_sequence == b->next_in_sequence;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFixSessionInfoTransferTails(UmiFixSessionInfo *value)
{
    (void)value;
    {
        size_t used = strlen(value->sender_comp_id) + 1U;
        memset(value->sender_comp_id + used, 0xa5, sizeof(value->sender_comp_id) - used);
    }
    {
        size_t used = strlen(value->target_comp_id) + 1U;
        memset(value->target_comp_id + used, 0xa5, sizeof(value->target_comp_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFixSessionInfoTransferMalformed(const UmiFixSessionInfo *sample)
{
    (void)sample;
    {
        UmiFixSessionInfo invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.sender_comp_id, 'x', sizeof(invalid.sender_comp_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_fix_session_info_valid(&invalid)) ||
            umi_fix_session_info_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated sender_comp_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFixSessionInfo invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target_comp_id, 'x', sizeof(invalid.target_comp_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_fix_session_info_valid(&invalid)) ||
            umi_fix_session_info_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target_comp_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFixSessionInfoTransferCases, UmiFixSessionInfo,
    umi_fix_session_info_archive_encode, umi_fix_session_info_archive_decode,
    UmiFixSessionInfoTransferEqual, UmiFixSessionInfoTransferTails, UmiFixSessionInfoTransferMalformed)

int main(void){
    int state=1;UmiVenueAdapter v={&state,"SIM",route};assert(umi_venue_adapter_valid(&v));
    UmiFixSessionInfo f={0};(void)snprintf(f.sender_comp_id,sizeof(f.sender_comp_id),"%s","UMI");(void)snprintf(f.target_comp_id,sizeof(f.target_comp_id),"%s","VENUE");f.next_out_sequence=1;f.next_in_sequence=1;assert(umi_fix_session_info_valid(&f));
    if (UmiFixSessionInfoTransferCases(&f) != 0) return 1;
return 0;
}
