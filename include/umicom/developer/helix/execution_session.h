/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/execution_session.h
 *
 * PURPOSE:
 *   Track the running execution cursor, completed steps, failures and cancellation state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_EXECUTION_SESSION_H
#define UMICOM_DEVELOPER_HELIX_EXECUTION_SESSION_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixExecutionSession { UmiHelixState state; size_t total_steps; size_t cursor; size_t completed; size_t failed; bool cancellation_requested; } UmiHelixExecutionSession;
/* Start a session with a fixed execution-step count. */
UmiStatus umi_helix_execution_session_start(UmiHelixExecutionSession *session,size_t total_steps);
/* Mark current step complete or failed and advance the cursor. */
UmiStatus umi_helix_execution_session_complete_step(UmiHelixExecutionSession *session,bool passed);
/* Request cooperative cancellation before the next mutation step. */
void umi_helix_execution_session_cancel(UmiHelixExecutionSession *session);
#ifdef __cplusplus
}
#endif
#endif
