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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/terminal_reconnect.h"
int main(void) { UmiTerminalRemoteTerminalReconnect v; umi_terminal_remote_terminal_reconnect_init(&v,100U,350U); if(umi_terminal_remote_terminal_reconnect_next_delay(&v)!=100U) return 1; if(umi_terminal_remote_terminal_reconnect_next_delay(&v)!=200U) return 2; if(umi_terminal_remote_terminal_reconnect_next_delay(&v)!=350U) return 3; umi_terminal_remote_terminal_reconnect_reset(&v); return umi_terminal_remote_terminal_reconnect_next_delay(&v)==100U?0:4; }
