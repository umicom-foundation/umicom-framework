/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/terminal_output_chunk.h
 *
 * PURPOSE:
 *   Model Framework-owned state for terminal output chunk with stable identity and deterministic score.
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
#ifndef UMICOM_TERMINAL_REMOTE_TERMINAL_OUTPUT_CHUNK_H
#define UMICOM_TERMINAL_REMOTE_TERMINAL_OUTPUT_CHUNK_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteTerminalOutputChunk { UmiTerminalRemoteNamedEntry identity; uint64_t metric; uint64_t limit; } UmiTerminalRemoteTerminalOutputChunk;
void umi_terminal_remote_terminal_output_chunk_init(UmiTerminalRemoteTerminalOutputChunk *value,const char *id,uint64_t limit);
UmiStatus umi_terminal_remote_terminal_output_chunk_set_metric(UmiTerminalRemoteTerminalOutputChunk *value,uint64_t metric);
bool umi_terminal_remote_terminal_output_chunk_within_limit(const UmiTerminalRemoteTerminalOutputChunk *value);
uint64_t umi_terminal_remote_terminal_output_chunk_score(const UmiTerminalRemoteTerminalOutputChunk *value);
#ifdef __cplusplus
}
#endif
#endif
