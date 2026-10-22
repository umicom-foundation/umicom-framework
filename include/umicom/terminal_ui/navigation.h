/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal_ui/navigation.h
 *
 * PURPOSE:
 *   Publish the public navigation contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_UI_NAVIGATION_H
#define UMICOM_TERMINAL_UI_NAVIGATION_H
#include "umicom/terminal/link.h"
#include "umicom/ui/navigation_stack.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the terminal ui link navigation operation used by this module and its client
 * applications.
 */
/** The fixed target array must contain a nonempty terminated string. An
 * unterminated target or one too large for the URI returns CAPACITY_EXCEEDED
 * without changing the output. The shortened label preserves UTF-8 character
 * boundaries for valid UTF-8 input; the full target stays in the URI. */
UmiStatus umi_terminal_ui_link_navigation(const UmiTerminalLink *link,
                                          UmiUiNavigationEntrySnapshot *out_entry);
#ifdef __cplusplus
}
#endif
#endif
