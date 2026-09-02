/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_terminal_persistence.c
 *
 * PURPOSE:
 *   Verify terminal persistence dirty state clears only after checkpoint.
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
#include "umicom/terminal/remote/terminal_persistence.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTerminalRemoteTerminalPersistence v; umi_terminal_remote_terminal_persistence_init(&v); /* Apply this branch only when its contract condition is satisfied. */ if(umi_terminal_remote_terminal_persistence_dirty(&v)) return 1; umi_terminal_remote_terminal_persistence_touch(&v); /* Apply this branch only when its contract condition is satisfied. */ if(!umi_terminal_remote_terminal_persistence_dirty(&v)) return 2; umi_terminal_remote_terminal_persistence_mark_saved(&v); return umi_terminal_remote_terminal_persistence_dirty(&v)?3:0; }
