/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_types.c
 *
 * PURPOSE:
 *   Verify stable shared terminal/remote types and deterministic fingerprinting.
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
#include "umicom/terminal/remote/types.h"
#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/terminal/remote/types.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiTerminalRemoteNamedEntryTransferEqual(const UmiTerminalRemoteNamedEntry *a, const UmiTerminalRemoteNamedEntry *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->label, b->label) == 0 &&
        a->revision == b->revision &&
        a->enabled == b->enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiTerminalRemoteNamedEntryTransferTails(UmiTerminalRemoteNamedEntry *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiTerminalRemoteNamedEntryTransferMalformed(const UmiTerminalRemoteNamedEntry *sample)
{
    (void)sample;
    {
        UmiTerminalRemoteNamedEntry invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_terminal_remote_named_entry_valid(&invalid)) ||
            umi_terminal_remote_named_entry_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiTerminalRemoteNamedEntry invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.label, 'x', sizeof(invalid.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_terminal_remote_named_entry_valid(&invalid)) ||
            umi_terminal_remote_named_entry_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated label was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiTerminalRemoteNamedEntryTransferCases, UmiTerminalRemoteNamedEntry,
    umi_terminal_remote_named_entry_archive_encode, umi_terminal_remote_named_entry_archive_decode,
    UmiTerminalRemoteNamedEntryTransferEqual, UmiTerminalRemoteNamedEntryTransferTails, UmiTerminalRemoteNamedEntryTransferMalformed)

int main(void)
{
    UmiTerminalRemoteNamedEntry entry;
    char out[8];
    umi_terminal_remote_named_entry_init(&entry, "term", "Terminal");
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_terminal_remote_named_entry_valid(&entry)) return 1;
    if (UmiTerminalRemoteNamedEntryTransferCases(&entry) != 0) return 1;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_terminal_remote_copy_text(out, sizeof(out), "remote") != UMI_STATUS_OK) return 2;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(out, "remote") != 0) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_terminal_remote_fingerprint_text("remote") == umi_terminal_remote_fingerprint_text("local")) return 4;
    return 0;
}
