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
#include "umicom/terminal/remote/types.h"
#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTerminalRemoteNamedEntry entry;
    char out[8];
    umi_terminal_remote_named_entry_init(&entry, "term", "Terminal");
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_terminal_remote_named_entry_valid(&entry)) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_terminal_remote_copy_text(out, sizeof(out), "remote") != UMI_STATUS_OK) return 2;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(out, "remote") != 0) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_terminal_remote_fingerprint_text("remote") == umi_terminal_remote_fingerprint_text("local")) return 4;
    return 0;
}
