/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_ibkr_boundary.c
 *
 * PURPOSE:
 *   Validate ibkr boundary behaviour in the trading foundation.
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
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/trading/ibkr_boundary.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiIbkrConnectionSettingsTransferEqual(const UmiIbkrConnectionSettings *a, const UmiIbkrConnectionSettings *b)
{
    return strcmp(a->host, b->host) == 0 &&
        a->port == b->port &&
        a->client_id == b->client_id &&
        a->environment == b->environment;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiIbkrConnectionSettingsTransferTails(UmiIbkrConnectionSettings *value)
{
    (void)value;
    {
        size_t used = strlen(value->host) + 1U;
        memset(value->host + used, 0xa5, sizeof(value->host) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiIbkrConnectionSettingsTransferMalformed(const UmiIbkrConnectionSettings *sample)
{
    (void)sample;
    {
        UmiIbkrConnectionSettings invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.host, 'x', sizeof(invalid.host));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ibkr_settings_valid(&invalid)) ||
            umi_ibkr_settings_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated host was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiIbkrConnectionSettingsTransferCases, UmiIbkrConnectionSettings,
    umi_ibkr_settings_archive_encode, umi_ibkr_settings_archive_decode,
    UmiIbkrConnectionSettingsTransferEqual, UmiIbkrConnectionSettingsTransferTails, UmiIbkrConnectionSettingsTransferMalformed)

int main(void){
    UmiIbkrConnectionSettings s={0};(void)snprintf(s.host,sizeof(s.host),"%s","127.0.0.1");s.port=7497U;s.client_id=17;s.environment=UMI_TRADING_PAPER;assert(umi_ibkr_settings_valid(&s));
    if (UmiIbkrConnectionSettingsTransferCases(&s) != 0) return 1;
return 0;
}
