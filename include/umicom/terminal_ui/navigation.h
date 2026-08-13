/* Toolkit-neutral navigation targets discovered in terminal output. */
#ifndef UMICOM_TERMINAL_UI_NAVIGATION_H
#define UMICOM_TERMINAL_UI_NAVIGATION_H
#include "umicom/terminal/link.h"
#include "umicom/ui/navigation_stack.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_terminal_ui_link_navigation(const UmiTerminalLink *link,
                                          UmiUiNavigationEntrySnapshot *out_entry);
#ifdef __cplusplus
}
#endif
#endif
