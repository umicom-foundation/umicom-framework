/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/audit_record.c
 *
 * PURPOSE:
 *   Create deterministic integration audit evidence without logging secrets or payload contents.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/audit_record.h"
#include <string.h>
#include <limits.h>


/*
 * Initialise fabric audit record from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_fabric_audit_record_init(UmiFabricAuditRecord *r,const char *record_id,const char *operation_id,const char *actor_id,UmiStatus status,uint64_t occurred_ms){UmiStatus s;uint64_t h=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(r,0,sizeof(*r));s=umi_fabric_copy_text(r->record_id,sizeof(r->record_id),record_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(r->operation_id,sizeof(r->operation_id),operation_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(r->actor_id,sizeof(r->actor_id),actor_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;r->status=status;r->occurred_ms=occurred_ms;h=umi_fabric_hash64(r->record_id,strlen(r->record_id),h);h=umi_fabric_hash64(r->operation_id,strlen(r->operation_id),h);h=umi_fabric_hash64(&r->occurred_ms,sizeof(r->occurred_ms),h);r->fingerprint=h;return UMI_STATUS_OK;}
