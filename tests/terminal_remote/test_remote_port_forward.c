/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_remote_port_forward.c
 *
 * PURPOSE:
 *   Verify port forwards require both ports and remote host.
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
#include "umicom/terminal/remote/remote_port_forward.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/terminal/remote/remote_port_forward.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiTerminalRemoteRemotePortForwardTransferEqual(const UmiTerminalRemoteRemotePortForward *a, const UmiTerminalRemoteRemotePortForward *b)
{
    return a->local_port == b->local_port &&
        a->remote_port == b->remote_port &&
        strcmp(a->remote_host, b->remote_host) == 0 &&
        a->enabled == b->enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiTerminalRemoteRemotePortForwardTransferTails(UmiTerminalRemoteRemotePortForward *value)
{
    (void)value;
    {
        size_t used = strlen(value->remote_host) + 1U;
        memset(value->remote_host + used, 0xa5, sizeof(value->remote_host) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiTerminalRemoteRemotePortForwardTransferMalformed(const UmiTerminalRemoteRemotePortForward *sample)
{
    (void)sample;
    {
        UmiTerminalRemoteRemotePortForward invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.remote_host, 'x', sizeof(invalid.remote_host));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_terminal_remote_remote_port_forward_valid(&invalid)) ||
            umi_terminal_remote_remote_port_forward_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated remote_host was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiTerminalRemoteRemotePortForwardTransferCases, UmiTerminalRemoteRemotePortForward,
    umi_terminal_remote_remote_port_forward_archive_encode, umi_terminal_remote_remote_port_forward_archive_decode,
    UmiTerminalRemoteRemotePortForwardTransferEqual, UmiTerminalRemoteRemotePortForwardTransferTails, UmiTerminalRemoteRemotePortForwardTransferMalformed)

int main(void) { UmiTerminalRemoteRemotePortForward v; umi_terminal_remote_remote_port_forward_init(&v,8080U,"127.0.0.1",80U); /* Apply this operation only while the related capability or state is available. */ if(!umi_terminal_remote_remote_port_forward_valid(&v)) return 1;
    if (UmiTerminalRemoteRemotePortForwardTransferCases(&v) != 0) return 1;
 umi_terminal_remote_remote_port_forward_init(&v,0U,"host",80U); return umi_terminal_remote_remote_port_forward_valid(&v)?2:0; }
