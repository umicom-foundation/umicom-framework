/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/manager.h
 *
 * PURPOSE:
 *   Own and resolve multiple terminal sessions for one application or workspace.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TERMINAL_MANAGER_H
#define UMICOM_TERMINAL_MANAGER_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/platform/clock.h"
#include "umicom/terminal/session.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTerminalManager UmiTerminalManager;

UmiStatus umi_terminal_manager_create(UmiClock *clock,
                                      UmiTerminalManager **out_manager);
void umi_terminal_manager_destroy(UmiTerminalManager *manager);
UmiStatus umi_terminal_manager_open(UmiTerminalManager *manager,
                                    const char *session_id,
                                    const char *title,
                                    const char *working_directory,
                                    UmiTerminalSession **out_session);
UmiStatus umi_terminal_manager_close(UmiTerminalManager *manager,
                                     const char *session_id);
UmiTerminalSession *umi_terminal_manager_find(UmiTerminalManager *manager,
                                              const char *session_id);
UmiTerminalSession *umi_terminal_manager_at(UmiTerminalManager *manager,
                                            size_t index);
size_t umi_terminal_manager_count(const UmiTerminalManager *manager);

#ifdef __cplusplus
}
#endif

#endif
