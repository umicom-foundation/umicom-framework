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
/**
 * Represent the terminal remote terminal output chunk data shared with callers of this
 * public contract.
 */
typedef struct UmiTerminalRemoteTerminalOutputChunk { UmiTerminalRemoteNamedEntry identity; uint64_t metric; uint64_t limit; } UmiTerminalRemoteTerminalOutputChunk;
/**
 * Initialise terminal remote terminal output chunk from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_terminal_output_chunk_init(UmiTerminalRemoteTerminalOutputChunk *value,const char *id,uint64_t limit);
/**
 * Provide the terminal remote terminal output chunk set metric operation used by this
 * module and its client applications.
 */
UmiStatus umi_terminal_remote_terminal_output_chunk_set_metric(UmiTerminalRemoteTerminalOutputChunk *value,uint64_t metric);
/**
 * Provide the terminal remote terminal output chunk within limit operation used by this
 * module and its client applications.
 */
bool umi_terminal_remote_terminal_output_chunk_within_limit(const UmiTerminalRemoteTerminalOutputChunk *value);
/**
 * Provide the terminal remote terminal output chunk score operation used by this module
 * and its client applications.
 */
uint64_t umi_terminal_remote_terminal_output_chunk_score(const UmiTerminalRemoteTerminalOutputChunk *value);
#ifdef __cplusplus
}
#endif
#endif
