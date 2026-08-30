/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/remote_handshake.h
 *
 * PURPOSE:
 *   Negotiate compatible protocol revisions and required remote capabilities.
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
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_HANDSHAKE_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_HANDSHAKE_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
bool umi_terminal_remote_remote_handshake_compatible(uint32_t local_revision,uint32_t remote_revision,uint64_t local_capabilities,uint64_t required_capabilities);
#ifdef __cplusplus
}
#endif
#endif
