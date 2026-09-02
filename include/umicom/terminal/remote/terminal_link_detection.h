/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/terminal_link_detection.h
 *
 * PURPOSE:
 *   Detect terminal links conservatively for navigation without shell ownership.
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
#ifndef UMICOM_TERMINAL_REMOTE_TERMINAL_LINK_DETECTION_H
#define UMICOM_TERMINAL_REMOTE_TERMINAL_LINK_DETECTION_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the terminal remote terminal link detection find first operation used by this
 * module and its client applications.
 */
size_t umi_terminal_remote_terminal_link_detection_find_first(const char *text,char *out,size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
