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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/types.h"
#include <string.h>
int main(void)
{
    UmiTerminalRemoteNamedEntry entry;
    char out[8];
    umi_terminal_remote_named_entry_init(&entry, "term", "Terminal");
    if (!umi_terminal_remote_named_entry_valid(&entry)) return 1;
    if (umi_terminal_remote_copy_text(out, sizeof(out), "remote") != UMI_STATUS_OK) return 2;
    if (strcmp(out, "remote") != 0) return 3;
    if (umi_terminal_remote_fingerprint_text("remote") == umi_terminal_remote_fingerprint_text("local")) return 4;
    return 0;
}
