/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_terminal_output_chunk.c
 *
 * PURPOSE:
 *   Verify terminal output chunk bounded state and deterministic score.
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
#include "umicom/terminal/remote/terminal_output_chunk.h"
int main(void) { UmiTerminalRemoteTerminalOutputChunk v; umi_terminal_remote_terminal_output_chunk_init(&v,"item",100U); if(umi_terminal_remote_terminal_output_chunk_set_metric(&v,50U)!=UMI_STATUS_OK) return 1; if(!umi_terminal_remote_terminal_output_chunk_within_limit(&v)||umi_terminal_remote_terminal_output_chunk_score(&v)==0U) return 2; if(umi_terminal_remote_terminal_output_chunk_set_metric(&v,101U)!=UMI_STATUS_OK) return 3; return umi_terminal_remote_terminal_output_chunk_within_limit(&v)?4:0; }
