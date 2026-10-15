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
#include "../../base/value_construction_internal.h"
#include <string.h>
#include <limits.h>


/*
 * Initialise fabric audit record from caller-provided values so later operations receive a
 * known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_fabric_audit_record_init(UmiFabricAuditRecord *r,const char *record_id,const char *operation_id,const char *actor_id,UmiStatus status,uint64_t occurred_ms){UmiStatus s;uint64_t h=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(r,0,sizeof(*r));s=umi_fabric_copy_text(r->record_id,sizeof(r->record_id),record_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(r->operation_id,sizeof(r->operation_id),operation_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(r->actor_id,sizeof(r->actor_id),actor_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;r->status=status;r->occurred_ms=occurred_ms;h=umi_fabric_hash64(r->record_id,strlen(r->record_id),h);h=umi_fabric_hash64(r->operation_id,strlen(r->operation_id),h);h=umi_fabric_hash64(&r->occurred_ms,sizeof(r->occurred_ms),h);r->fingerprint=h;return UMI_STATUS_OK;}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_fabric_audit_record_init_build_value(UmiFabricAuditRecord *r,const char *record_id,const char *operation_id,const char *actor_id,UmiStatus status,uint64_t occurred_ms){UmiStatus s;uint64_t h=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(r,0,sizeof(*r));s=umi_fabric_copy_text(r->record_id,sizeof(r->record_id),record_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(r->operation_id,sizeof(r->operation_id),operation_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(r->actor_id,sizeof(r->actor_id),actor_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;r->status=status;r->occurred_ms=occurred_ms;h=umi_fabric_hash64(r->record_id,strlen(r->record_id),h);h=umi_fabric_hash64(r->operation_id,strlen(r->operation_id),h);h=umi_fabric_hash64(&r->occurred_ms,sizeof(r->occurred_ms),h);r->fingerprint=h;return UMI_STATUS_OK;}

UMI_DEFINE_STAGED_VALUE_INIT(umi_fabric_audit_record_init, UmiFabricAuditRecord, r,
    (UmiFabricAuditRecord *r, const char *record_id, const char *operation_id, const char *actor_id, UmiStatus status, uint64_t occurred_ms),
    umi_fabric_audit_record_init_build_value(candidate, record_id, operation_id, actor_id, status, occurred_ms))
