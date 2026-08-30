/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/reporting_audit.c
 *
 * PURPOSE:
 *   Maintain ordered audit evidence for reporting operations.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/regulatory/reporting_audit.h"

#include <string.h>
void umi_reg_reporting_audit_init(UmiReportingAudit *audit){ if(audit!=NULL)memset(audit,0,sizeof *audit); }
UmiStatus umi_reg_reporting_audit_append(UmiReportingAudit *audit,uint64_t sequence,const char *action,uint64_t evidence_hash){ UmiStatus s; UmiReportingAuditEvent *e; if(audit==NULL||sequence==0U||evidence_hash==0U)return UMI_STATUS_INVALID_ARGUMENT; if(audit->count>=UMI_REG_MAX_HISTORY)return UMI_STATUS_CAPACITY_EXCEEDED; if(audit->count>0U&&sequence<=audit->events[audit->count-1U].sequence)return UMI_STATUS_INVALID_STATE; e=&audit->events[audit->count]; memset(e,0,sizeof *e); s=umi_reg_copy_text(e->action,sizeof e->action,action); if(s!=UMI_STATUS_OK)return s; e->sequence=sequence;e->evidence_hash=evidence_hash;++audit->count;return UMI_STATUS_OK; }
