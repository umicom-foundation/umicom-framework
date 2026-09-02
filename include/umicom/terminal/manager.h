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

/**
 * Represent the terminal manager data shared with callers of this public contract.
 */
typedef struct UmiTerminalManager UmiTerminalManager;

/**
 * Initialise terminal manager from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_terminal_manager_create(UmiClock *clock,
                                      UmiTerminalManager **out_manager);
/**
 * Release or reset state held by terminal manager so the same storage can be reused
 * safely.
 */
void umi_terminal_manager_destroy(UmiTerminalManager *manager);
/**
 * Provide the terminal manager open operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_manager_open(UmiTerminalManager *manager,
                                    const char *session_id,
                                    const char *title,
                                    const char *working_directory,
                                    UmiTerminalSession **out_session);
/**
 * Provide the terminal manager close operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_manager_close(UmiTerminalManager *manager,
                                     const char *session_id);
/**
 * Find terminal manager while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiTerminalSession *umi_terminal_manager_find(UmiTerminalManager *manager,
                                              const char *session_id);
/**
 * Find terminal manager while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiTerminalSession *umi_terminal_manager_at(UmiTerminalManager *manager,
                                            size_t index);
/**
 * Return the number of records represented by terminal manager without changing their
 * state.
 */
size_t umi_terminal_manager_count(const UmiTerminalManager *manager);

#ifdef __cplusplus
}
#endif

#endif
