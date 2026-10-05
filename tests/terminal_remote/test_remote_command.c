/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_remote_command.c
 *
 * PURPOSE:
 *   Verify remote commands require explicit program and working directory.
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
#include "umicom/terminal/remote/remote_command.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/terminal/remote/remote_command.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiTerminalRemoteRemoteCommandTransferEqual(const UmiTerminalRemoteRemoteCommand *a, const UmiTerminalRemoteRemoteCommand *b)
{
    return strcmp(a->program, b->program) == 0 &&
        strcmp(a->working_directory, b->working_directory) == 0 &&
        a->interactive == b->interactive;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiTerminalRemoteRemoteCommandTransferTails(UmiTerminalRemoteRemoteCommand *value)
{
    (void)value;
    {
        size_t used = strlen(value->program) + 1U;
        memset(value->program + used, 0xa5, sizeof(value->program) - used);
    }
    {
        size_t used = strlen(value->working_directory) + 1U;
        memset(value->working_directory + used, 0xa5, sizeof(value->working_directory) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiTerminalRemoteRemoteCommandTransferMalformed(const UmiTerminalRemoteRemoteCommand *sample)
{
    (void)sample;
    {
        UmiTerminalRemoteRemoteCommand invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.program, 'x', sizeof(invalid.program));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_terminal_remote_remote_command_valid(&invalid)) ||
            umi_terminal_remote_remote_command_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated program was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiTerminalRemoteRemoteCommand invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.working_directory, 'x', sizeof(invalid.working_directory));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_terminal_remote_remote_command_valid(&invalid)) ||
            umi_terminal_remote_remote_command_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated working_directory was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiTerminalRemoteRemoteCommandTransferCases, UmiTerminalRemoteRemoteCommand,
    umi_terminal_remote_remote_command_archive_encode, umi_terminal_remote_remote_command_archive_decode,
    UmiTerminalRemoteRemoteCommandTransferEqual, UmiTerminalRemoteRemoteCommandTransferTails, UmiTerminalRemoteRemoteCommandTransferMalformed)

int main(void) { UmiTerminalRemoteRemoteCommand v; umi_terminal_remote_remote_command_init(&v,"cmake","/work",false); /* Apply this operation only while the related capability or state is available. */ if(!umi_terminal_remote_remote_command_valid(&v)) return 1;
    if (UmiTerminalRemoteRemoteCommandTransferCases(&v) != 0) return 1;
 umi_terminal_remote_remote_command_init(&v,"","/work",false); return umi_terminal_remote_remote_command_valid(&v)?2:0; }
