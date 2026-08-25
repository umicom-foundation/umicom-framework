/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_remote_language_bridge.c
 *
 * PURPOSE:
 *   Verify remote language bridge requires two distinct stable identities.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/remote_language_bridge.h"
int main(void) { UmiTerminalRemoteRemoteLanguageBridge v; umi_terminal_remote_remote_language_bridge_init(&v,"left","right"); if(!umi_terminal_remote_remote_language_bridge_valid(&v)) return 1; if(umi_terminal_remote_remote_language_bridge_fingerprint(&v)==0U) return 2; umi_terminal_remote_remote_language_bridge_init(&v,"same","same"); return umi_terminal_remote_remote_language_bridge_valid(&v)?3:0; }
