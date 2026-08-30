/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/audit_record.h
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
#ifndef UMICOM_INTEGRATION_FABRIC_AUDIT_RECORD_H
#define UMICOM_INTEGRATION_FABRIC_AUDIT_RECORD_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiFabricAuditRecord { char record_id[UMI_FABRIC_ID_CAPACITY]; char operation_id[UMI_FABRIC_ID_CAPACITY]; char actor_id[UMI_FABRIC_ID_CAPACITY]; UmiStatus status; uint64_t occurred_ms; uint64_t fingerprint; } UmiFabricAuditRecord;
UmiStatus umi_fabric_audit_record_init(UmiFabricAuditRecord *record,const char *record_id,const char *operation_id,const char *actor_id,UmiStatus status,uint64_t occurred_ms);

#ifdef __cplusplus
}
#endif
#endif
