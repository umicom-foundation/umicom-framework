/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_terminal_task_binding.c
 *
 * PURPOSE:
 *   Verify terminal task binding requires two distinct stable identities.
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
#include "umicom/terminal/remote/terminal_task_binding.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/terminal/remote/terminal_task_binding.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiTerminalRemoteTerminalTaskBindingTransferEqual(const UmiTerminalRemoteTerminalTaskBinding *a, const UmiTerminalRemoteTerminalTaskBinding *b)
{
    return strcmp(a->left_id, b->left_id) == 0 &&
        strcmp(a->right_id, b->right_id) == 0 &&
        a->revision == b->revision &&
        a->enabled == b->enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiTerminalRemoteTerminalTaskBindingTransferTails(UmiTerminalRemoteTerminalTaskBinding *value)
{
    (void)value;
    {
        size_t used = strlen(value->left_id) + 1U;
        memset(value->left_id + used, 0xa5, sizeof(value->left_id) - used);
    }
    {
        size_t used = strlen(value->right_id) + 1U;
        memset(value->right_id + used, 0xa5, sizeof(value->right_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiTerminalRemoteTerminalTaskBindingTransferMalformed(const UmiTerminalRemoteTerminalTaskBinding *sample)
{
    (void)sample;
    {
        UmiTerminalRemoteTerminalTaskBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.left_id, 'x', sizeof(invalid.left_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_terminal_remote_terminal_task_binding_valid(&invalid)) ||
            umi_terminal_remote_terminal_task_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated left_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiTerminalRemoteTerminalTaskBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.right_id, 'x', sizeof(invalid.right_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_terminal_remote_terminal_task_binding_valid(&invalid)) ||
            umi_terminal_remote_terminal_task_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated right_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiTerminalRemoteTerminalTaskBindingTransferCases, UmiTerminalRemoteTerminalTaskBinding,
    umi_terminal_remote_terminal_task_binding_archive_encode, umi_terminal_remote_terminal_task_binding_archive_decode,
    UmiTerminalRemoteTerminalTaskBindingTransferEqual, UmiTerminalRemoteTerminalTaskBindingTransferTails, UmiTerminalRemoteTerminalTaskBindingTransferMalformed)

int main(void) { UmiTerminalRemoteTerminalTaskBinding v; umi_terminal_remote_terminal_task_binding_init(&v,"left","right"); /* Apply this operation only while the related capability or state is available. */ if(!umi_terminal_remote_terminal_task_binding_valid(&v)) return 1;
    if (UmiTerminalRemoteTerminalTaskBindingTransferCases(&v) != 0) return 1;
 /* Apply this operation only while the related capability or state is available. */ if(umi_terminal_remote_terminal_task_binding_fingerprint(&v)==0U) return 2; umi_terminal_remote_terminal_task_binding_init(&v,"same","same"); return umi_terminal_remote_terminal_task_binding_valid(&v)?3:0; }
