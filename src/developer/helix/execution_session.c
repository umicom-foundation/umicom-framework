/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/helix/execution_session.c
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
#include "umicom/developer/helix/execution_session.h"
#include <string.h>
/* Enter running state only when there is executable work. */
UmiStatus umi_helix_execution_session_start(UmiHelixExecutionSession *s,size_t total){ if(s==NULL||total==0U){ return UMI_STATUS_INVALID_ARGUMENT; } memset(s,0,sizeof(*s)); s->state=UMI_HELIX_RUNNING; s->total_steps=total; return UMI_STATUS_OK; }
/* Advance one step and terminate conservatively on failure or cancellation. */
UmiStatus umi_helix_execution_session_complete_step(UmiHelixExecutionSession *s,bool passed){ if(s==NULL||s->state!=UMI_HELIX_RUNNING||s->cursor>=s->total_steps){ return UMI_STATUS_INVALID_STATE; } if(s->cancellation_requested){s->state=UMI_HELIX_CANCELLED;return UMI_STATUS_CANCELLED;} s->cursor++; if(passed)s->completed++;else{s->failed++;s->state=UMI_HELIX_FAILED;return UMI_STATUS_INVALID_STATE;} if(s->cursor==s->total_steps)s->state=UMI_HELIX_SUCCEEDED; return UMI_STATUS_OK; }
/* Set a cooperative cancellation flag without interrupting an in-flight side effect. */
void umi_helix_execution_session_cancel(UmiHelixExecutionSession *s){ if(s!=NULL) s->cancellation_requested=true; }
