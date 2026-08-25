/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/compensation_record.h
 *
 * PURPOSE:
 *   Record compensation attempts and final disposition for audit/recovery.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_COMPENSATION_RECORD_H
#define UMICOM_INTEGRATION_FABRIC_COMPENSATION_RECORD_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiFabricCompensationRecord { char workflow_id[UMI_FABRIC_ID_CAPACITY]; char step_id[UMI_FABRIC_ID_CAPACITY]; uint32_t attempts; bool succeeded; uint64_t updated_ms; } UmiFabricCompensationRecord;
UmiStatus umi_fabric_compensation_record_init(UmiFabricCompensationRecord *record,const char *workflow_id,const char *step_id);
void umi_fabric_compensation_record_attempt(UmiFabricCompensationRecord *record,bool success,uint64_t now_ms);

#ifdef __cplusplus
}
#endif
#endif
