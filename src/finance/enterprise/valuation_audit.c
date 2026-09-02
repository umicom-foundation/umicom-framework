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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/valuation_audit.h"

#include <string.h>
/*
 * Initialise enterprise valuation audit from caller-provided values so later operations
 * receive a known state.
 */
void umi_enterprise_valuation_audit_init(UmiEnterpriseValuationAudit *a){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(a!=NULL)memset(a,0,sizeof *a);}
/*
 * Add enterprise valuation audit only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_enterprise_valuation_audit_append(UmiEnterpriseValuationAudit *a,const UmiEnterpriseValuationAuditEvent *e){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(a==NULL||e==NULL||e->sequence==0U||e->event_time_ms<0)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(a->count>=UMI_ENTERPRISE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(a->count>0U&&(e->sequence<=a->events[a->count-1U].sequence||e->event_time_ms<a->events[a->count-1U].event_time_ms))return UMI_STATUS_INVALID_STATE;a->events[a->count++]=*e;return UMI_STATUS_OK;}
