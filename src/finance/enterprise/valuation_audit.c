/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/valuation_audit.c
 *
 * PURPOSE:
 *   Record ordered valuation control-plane events for operational audit.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/valuation_audit.h"

#include <string.h>
void umi_enterprise_valuation_audit_init(UmiEnterpriseValuationAudit *a){if(a!=NULL)memset(a,0,sizeof *a);}
UmiStatus umi_enterprise_valuation_audit_append(UmiEnterpriseValuationAudit *a,const UmiEnterpriseValuationAuditEvent *e){if(a==NULL||e==NULL||e->sequence==0U||e->event_time_ms<0)return UMI_STATUS_INVALID_ARGUMENT;if(a->count>=UMI_ENTERPRISE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;if(a->count>0U&&(e->sequence<=a->events[a->count-1U].sequence||e->event_time_ms<a->events[a->count-1U].event_time_ms))return UMI_STATUS_INVALID_STATE;a->events[a->count++]=*e;return UMI_STATUS_OK;}
