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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/remote_port_forward.h"
int main(void) { UmiTerminalRemoteRemotePortForward v; umi_terminal_remote_remote_port_forward_init(&v,8080U,"127.0.0.1",80U); if(!umi_terminal_remote_remote_port_forward_valid(&v)) return 1; umi_terminal_remote_remote_port_forward_init(&v,0U,"host",80U); return umi_terminal_remote_remote_port_forward_valid(&v)?2:0; }
