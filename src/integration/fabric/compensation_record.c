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
#include <string.h>
#include <limits.h>


UmiStatus umi_fabric_compensation_record_init(UmiFabricCompensationRecord *record,const char *workflow_id,const char *step_id){UmiStatus s;if(record==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(record,0,sizeof(*record));s=umi_fabric_copy_text(record->workflow_id,sizeof(record->workflow_id),workflow_id);if(s!=UMI_STATUS_OK)return s;return umi_fabric_copy_text(record->step_id,sizeof(record->step_id),step_id);}
void umi_fabric_compensation_record_attempt(UmiFabricCompensationRecord *record,bool success,uint64_t now_ms){if(record!=NULL){if(record->attempts<UINT32_MAX)record->attempts++;record->succeeded=success;record->updated_ms=now_ms;}}
