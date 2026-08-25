/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_remote_endpoint.c
 *
 * PURPOSE:
 *   Verify endpoint requires a host and non-zero port.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/remote_endpoint.h"
int main(void) { UmiTerminalRemoteRemoteEndpoint v; umi_terminal_remote_remote_endpoint_init(&v,"host",22U,true); if(!umi_terminal_remote_remote_endpoint_valid(&v)) return 1; umi_terminal_remote_remote_endpoint_init(&v,"host",0U,true); return umi_terminal_remote_remote_endpoint_valid(&v)?2:0; }
