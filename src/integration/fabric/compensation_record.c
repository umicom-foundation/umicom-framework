/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/compensation_record.c
 *
 * PURPOSE:
 *   Record compensation attempts and final disposition for audit/recovery.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/compensation_record.h"
#include "../../base/value_construction_internal.h"
#include <string.h>
#include <limits.h>


/*
 * Initialise fabric compensation record from caller-provided values so later operations
 * receive a known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_fabric_compensation_record_init(UmiFabricCompensationRecord *record,const char *workflow_id,const char *step_id){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(record==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(record,0,sizeof(*record));s=umi_fabric_copy_text(record->workflow_id,sizeof(record->workflow_id),workflow_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;return umi_fabric_copy_text(record->step_id,sizeof(record->step_id),step_id);}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_fabric_compensation_record_init_build_value(UmiFabricCompensationRecord *record,const char *workflow_id,const char *step_id){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(record==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(record,0,sizeof(*record));s=umi_fabric_copy_text(record->workflow_id,sizeof(record->workflow_id),workflow_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;return umi_fabric_copy_text(record->step_id,sizeof(record->step_id),step_id);}

UMI_DEFINE_STAGED_VALUE_INIT(umi_fabric_compensation_record_init, UmiFabricCompensationRecord, record,
    (UmiFabricCompensationRecord *record, const char *workflow_id, const char *step_id),
    umi_fabric_compensation_record_init_build_value(candidate, workflow_id, step_id))

/*
 * Provide the fabric compensation record attempt operation used by this module and its
 * client applications.
 */
void umi_fabric_compensation_record_attempt(UmiFabricCompensationRecord *record,bool success,uint64_t now_ms){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(record!=NULL){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(record->attempts<UINT32_MAX)record->attempts++;record->succeeded=success;record->updated_ms=now_ms;}}
