/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/terminal_encoding.h
 *
 * PURPOSE:
 *   Resolve terminal text encoding support without frontend-specific rules.
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
#ifndef UMICOM_TERMINAL_REMOTE_TERMINAL_ENCODING_H
#define UMICOM_TERMINAL_REMOTE_TERMINAL_ENCODING_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the terminal remote terminal encoding supported operation used by this module
 * and its client applications.
 */
bool umi_terminal_remote_terminal_encoding_supported(const char *name);
#ifdef __cplusplus
}
#endif
#endif
