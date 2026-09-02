/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/helix/helix_service.c
 *
 * PURPOSE:
 *   Provide the top-level Framework-owned Helix control-plane state and guarded session transitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/helix_service.h"
#include <string.h>
/* Default to supervised autonomy and disabled mutation. */
void umi_helix_service_init(UmiHelixService *s){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL){ return; } memset(s,0,sizeof(*s)); s->autonomy_ceiling=UMI_HELIX_AUTONOMY_SUPERVISED; s->repository_mutation_enabled=false; }
/* Allocate a session from bounded Framework-owned storage. */
UmiStatus umi_helix_service_open(UmiHelixService *s,const char *id,uint32_t max_iter,UmiHelixSession **out){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||out==NULL){ return UMI_STATUS_INVALID_ARGUMENT; } /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->count>=UMI_HELIX_MAX_ITEMS){ return UMI_STATUS_CAPACITY_EXCEEDED; } /* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0;i<s->count;i++) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(s->sessions[i].id,id)==0){ return UMI_STATUS_ALREADY_EXISTS; } UmiStatus st=umi_helix_session_init(&s->sessions[s->count],id,max_iter); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK){ return st; } s->sessions[s->count].autonomy=s->autonomy_ceiling; *out=&s->sessions[s->count++]; return UMI_STATUS_OK; }
/* Require an explicit host decision before any repository mutation can be considered. */
void umi_helix_service_set_mutation(UmiHelixService *s,bool enabled){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL) s->repository_mutation_enabled=enabled; }
/* Combine control-plane and session approval/checkpoint evidence conservatively. */
UmiHelixDecision umi_helix_service_execution_decision(const UmiHelixService *s,const UmiHelixSession *session){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||session==NULL){ return UMI_HELIX_DECISION_DENY; } /* Protect caller-owned memory by checking that required state is available before it is used. */ if(!s->repository_mutation_enabled){ return UMI_HELIX_DECISION_REVIEW; } /* Protect caller-owned memory by checking that required state is available before it is used. */ if(session->blockers>0U||!session->checkpoint_ready){ return UMI_HELIX_DECISION_DENY; } /* Protect caller-owned memory by checking that required state is available before it is used. */ if(!session->approved){ return UMI_HELIX_DECISION_REVIEW; } /* Protect caller-owned memory by checking that required state is available before it is used. */ if(session->autonomy>s->autonomy_ceiling){ return UMI_HELIX_DECISION_DENY; } return UMI_HELIX_DECISION_ALLOW; }
