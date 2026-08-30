/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/helix/helix_session.c
 *
 * PURPOSE:
 *   Aggregate goal, plan, budgets, checkpoints, validation and lifecycle state for one Helix run.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/helix_session.h"
#include <string.h>
/* Create a conservative supervised session that cannot mutate without approval/checkpoint evidence. */
UmiStatus umi_helix_session_init(UmiHelixSession *s,const char *id,uint32_t max_iterations){ if(s==NULL||!umi_helix_valid_id(id)||max_iterations==0U){ return UMI_STATUS_INVALID_ARGUMENT; } memset(s,0,sizeof(*s)); s->structure_size=(uint32_t)sizeof(*s); s->api_version=UMI_HELIX_API_VERSION; (void)umi_helix_copy_text(s->id,sizeof(s->id),id); s->state=UMI_HELIX_IDLE; s->autonomy=UMI_HELIX_AUTONOMY_SUPERVISED; s->max_iterations=max_iterations; return UMI_STATUS_OK; }
/* Enforce explicit lifecycle ordering and guarded entry to running state. */
UmiStatus umi_helix_session_transition(UmiHelixSession *s,UmiHelixState next){ if(s==NULL){ return UMI_STATUS_INVALID_ARGUMENT; } bool ok=false; if(s->state==UMI_HELIX_IDLE&&next==UMI_HELIX_PLANNED) ok=true; else if(s->state==UMI_HELIX_PLANNED&&next==UMI_HELIX_READY) ok=s->blockers==0U; else if(s->state==UMI_HELIX_READY&&next==UMI_HELIX_RUNNING) ok=s->approved&&s->checkpoint_ready; else if(s->state==UMI_HELIX_RUNNING&&(next==UMI_HELIX_SUCCEEDED||next==UMI_HELIX_FAILED||next==UMI_HELIX_BLOCKED||next==UMI_HELIX_CANCELLED)) ok=true; else if((s->state==UMI_HELIX_FAILED||s->state==UMI_HELIX_BLOCKED)&&next==UMI_HELIX_ROLLED_BACK) ok=s->checkpoint_ready; if(!ok){ return UMI_STATUS_INVALID_STATE; } s->state=next; if(next==UMI_HELIX_RUNNING) s->iteration++; return UMI_STATUS_OK; }
/* Check state and budget before permitting another controlled iteration. */
bool umi_helix_session_can_iterate(const UmiHelixSession *s){ return s!=NULL&&s->iteration<s->max_iterations&&(s->state==UMI_HELIX_PLANNED||s->state==UMI_HELIX_READY||s->state==UMI_HELIX_FAILED||s->state==UMI_HELIX_BLOCKED); }
