/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_remote_toolchain.c
 *
 * PURPOSE:
 *   Verify remote toolchain requires two distinct stable identities.
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
#include "umicom/terminal/remote/remote_toolchain.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTerminalRemoteRemoteToolchain v; umi_terminal_remote_remote_toolchain_init(&v,"left","right"); /* Apply this operation only while the related capability or state is available. */ if(!umi_terminal_remote_remote_toolchain_valid(&v)) return 1; /* Apply this operation only while the related capability or state is available. */ if(umi_terminal_remote_remote_toolchain_fingerprint(&v)==0U) return 2; umi_terminal_remote_remote_toolchain_init(&v,"same","same"); return umi_terminal_remote_remote_toolchain_valid(&v)?3:0; }
