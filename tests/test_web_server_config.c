/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_web_server_config.c
 *
 * PURPOSE:
 *   Verify one part of the Web Server and multi-frontend platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This small test exercises the public contract directly so a beginner can see the expected behaviour without starting a complete Umicom product.
 */

/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/umicom.h"
#include <assert.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "value_archive/transfer_cases.h"

#include "umicom/web/server_config.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWebServerConfigTransferEqual(const UmiWebServerConfig *a, const UmiWebServerConfig *b)
{
    return strcmp(a->bind_address, b->bind_address) == 0 &&
        a->port == b->port &&
        a->max_request_bytes == b->max_request_bytes &&
        a->loopback_only == b->loopback_only;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiWebServerConfigTransferTails(UmiWebServerConfig *value)
{
    (void)value;
    {
        size_t used = strlen(value->bind_address) + 1U;
        memset(value->bind_address + used, 0xa5, sizeof(value->bind_address) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiWebServerConfigTransferMalformed(const UmiWebServerConfig *sample)
{
    (void)sample;
    {
        UmiWebServerConfig invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.bind_address, 'x', sizeof(invalid.bind_address));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_web_server_config_validate(&invalid) != UMI_STATUS_OK) ||
            umi_web_server_config_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated bind_address was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWebServerConfigTransferCases, UmiWebServerConfig,
    umi_web_server_config_archive_encode, umi_web_server_config_archive_decode,
    UmiWebServerConfigTransferEqual, UmiWebServerConfigTransferTails, UmiWebServerConfigTransferMalformed)

int main(void){UmiWebServerConfig c=umi_web_server_config_default();UmiWebServerState s;assert(umi_web_server_config_validate(&c)==UMI_STATUS_OK);
    if (UmiWebServerConfigTransferCases(&c) != 0) return 1;
umi_web_server_state_init(&s);assert(s.phase==UMI_WEB_SERVER_STOPPED);return 0;}
