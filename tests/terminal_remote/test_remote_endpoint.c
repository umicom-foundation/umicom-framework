/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_remote_endpoint.c
 *
 * PURPOSE:
 *   Verify endpoint requires a host and non-zero port.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
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
#include "umicom/terminal/remote/remote_endpoint.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/terminal/remote/remote_endpoint.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiTerminalRemoteRemoteEndpointTransferEqual(const UmiTerminalRemoteRemoteEndpoint *a, const UmiTerminalRemoteRemoteEndpoint *b)
{
    return strcmp(a->host, b->host) == 0 &&
        a->port == b->port &&
        a->secure == b->secure;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiTerminalRemoteRemoteEndpointTransferTails(UmiTerminalRemoteRemoteEndpoint *value)
{
    (void)value;
    {
        size_t used = strlen(value->host) + 1U;
        memset(value->host + used, 0xa5, sizeof(value->host) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiTerminalRemoteRemoteEndpointTransferMalformed(const UmiTerminalRemoteRemoteEndpoint *sample)
{
    (void)sample;
    {
        UmiTerminalRemoteRemoteEndpoint invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.host, 'x', sizeof(invalid.host));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_terminal_remote_remote_endpoint_valid(&invalid)) ||
            umi_terminal_remote_remote_endpoint_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated host was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiTerminalRemoteRemoteEndpointTransferCases, UmiTerminalRemoteRemoteEndpoint,
    umi_terminal_remote_remote_endpoint_archive_encode, umi_terminal_remote_remote_endpoint_archive_decode,
    UmiTerminalRemoteRemoteEndpointTransferEqual, UmiTerminalRemoteRemoteEndpointTransferTails, UmiTerminalRemoteRemoteEndpointTransferMalformed)

int main(void) { UmiTerminalRemoteRemoteEndpoint v; umi_terminal_remote_remote_endpoint_init(&v,"host",22U,true); /* Apply this operation only while the related capability or state is available. */ if(!umi_terminal_remote_remote_endpoint_valid(&v)) return 1;
    if (UmiTerminalRemoteRemoteEndpointTransferCases(&v) != 0) return 1;
 umi_terminal_remote_remote_endpoint_init(&v,"host",0U,true); return umi_terminal_remote_remote_endpoint_valid(&v)?2:0; }
