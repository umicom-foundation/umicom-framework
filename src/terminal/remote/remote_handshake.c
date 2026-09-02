/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/remote_handshake.c
 *
 * PURPOSE:
 *   Implement conservative remote handshake compatibility checks.
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
#include "umicom/terminal/remote/remote_handshake.h"
/*
 * Provide the terminal remote remote handshake compatible operation used by this module
 * and its client applications.
 */
bool umi_terminal_remote_remote_handshake_compatible(uint32_t local_revision,uint32_t remote_revision,uint64_t local_capabilities,uint64_t required_capabilities) { return local_revision!=0U&&remote_revision!=0U&&local_revision==remote_revision&&(local_capabilities&required_capabilities)==required_capabilities; }
