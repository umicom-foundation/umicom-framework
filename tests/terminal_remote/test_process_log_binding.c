/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_process_log_binding.c
 *
 * PURPOSE:
 *   Verify process log binding requires two distinct stable identities.
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
#include "umicom/terminal/remote/process_log_binding.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTerminalRemoteProcessLogBinding v; umi_terminal_remote_process_log_binding_init(&v,"left","right"); /* Apply this operation only while the related capability or state is available. */ if(!umi_terminal_remote_process_log_binding_valid(&v)) return 1; /* Apply this operation only while the related capability or state is available. */ if(umi_terminal_remote_process_log_binding_fingerprint(&v)==0U) return 2; umi_terminal_remote_process_log_binding_init(&v,"same","same"); return umi_terminal_remote_process_log_binding_valid(&v)?3:0; }
