/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/risk_decision.c
 *
 * PURPOSE:
 *   Create allow/deny risk decisions with bounded human-readable reasons.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of risk decision. Product-specific UI and vendor details stay outside this file.
 */

#include <stdio.h>
#include "umicom/trading/risk_decision.h"
void umi_risk_decision_allow(UmiRiskDecision *d){if(d!=NULL){d->allowed=1;d->reason[0]='\0';}}
void umi_risk_decision_deny(UmiRiskDecision *d,const char *reason){if(d!=NULL){d->allowed=0;(void)snprintf(d->reason,sizeof(d->reason),"%s",reason!=NULL?reason:"risk check failed");}}
