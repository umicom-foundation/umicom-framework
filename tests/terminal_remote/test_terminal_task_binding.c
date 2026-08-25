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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/terminal_task_binding.h"
int main(void) { UmiTerminalRemoteTerminalTaskBinding v; umi_terminal_remote_terminal_task_binding_init(&v,"left","right"); if(!umi_terminal_remote_terminal_task_binding_valid(&v)) return 1; if(umi_terminal_remote_terminal_task_binding_fingerprint(&v)==0U) return 2; umi_terminal_remote_terminal_task_binding_init(&v,"same","same"); return umi_terminal_remote_terminal_task_binding_valid(&v)?3:0; }
