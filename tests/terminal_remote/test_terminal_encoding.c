/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_terminal_encoding.c
 *
 * PURPOSE:
 *   Verify terminal encoding allow-list.
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
#include "umicom/terminal/remote/terminal_encoding.h"
int main(void) { if(!umi_terminal_remote_terminal_encoding_supported("UTF-8")) return 1; return umi_terminal_remote_terminal_encoding_supported("EBCDIC")?2:0; }
