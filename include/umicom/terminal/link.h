/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/link.h
 *
 * PURPOSE:
 *   Detect web, file and compiler-location links in terminal output without
 *   binding navigation to a graphical toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_LINK_H
#define UMICOM_TERMINAL_LINK_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/terminal/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named terminal link kind values accepted by this public contract.
 */
typedef enum UmiTerminalLinkKind {
    UMI_TERMINAL_LINK_WEB = 0,
    UMI_TERMINAL_LINK_FILE = 1,
    UMI_TERMINAL_LINK_COMPILER_LOCATION = 2,
    UMI_TERMINAL_LINK_EMAIL = 3
} UmiTerminalLinkKind;

/**
 * Represent the terminal link data shared with callers of this public contract.
 */
typedef struct UmiTerminalLink {
    UmiTerminalLinkKind kind;
    size_t byte_offset;
    size_t byte_length;
    char target[UMI_TERMINAL_PATH_CAPACITY];
    uint32_t line;
    uint32_t column;
} UmiTerminalLink;

/**
 * Represent the terminal link result data shared with callers of this public contract.
 */
typedef struct UmiTerminalLinkResult {
    UmiTerminalLink links[UMI_TERMINAL_LINK_MAX];
    size_t count;
    int truncated;
} UmiTerminalLinkResult;

/**
 * Provide the terminal links detect operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_links_detect(const char *text,
                                    UmiTerminalLinkResult *out_result);
/**
 * Provide the terminal link kind text operation used by this module and its client
 * applications.
 */
const char *umi_terminal_link_kind_text(UmiTerminalLinkKind kind);

#ifdef __cplusplus
}
#endif
#endif
