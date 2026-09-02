/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_terminal_reconnect.c
 *
 * PURPOSE:
 *   Verify bounded exponential retry delays for terminal reconnect.
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
#include "umicom/terminal/remote/terminal_reconnect.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTerminalRemoteTerminalReconnect v; umi_terminal_remote_terminal_reconnect_init(&v,100U,350U); /* Apply this branch only when its contract condition is satisfied. */ if(umi_terminal_remote_terminal_reconnect_next_delay(&v)!=100U) return 1; /* Apply this branch only when its contract condition is satisfied. */ if(umi_terminal_remote_terminal_reconnect_next_delay(&v)!=200U) return 2; /* Apply this branch only when its contract condition is satisfied. */ if(umi_terminal_remote_terminal_reconnect_next_delay(&v)!=350U) return 3; umi_terminal_remote_terminal_reconnect_reset(&v); return umi_terminal_remote_terminal_reconnect_next_delay(&v)==100U?0:4; }
