/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/regulatory_service.c
 *
 * PURPOSE:
 *   Provide aggregate readiness for Framework-owned regulatory reporting and governance services.
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

#include "umicom/finance/regulatory/regulatory_service.h"

#include <string.h>
/*
 * Initialise reg regulatory service from caller-provided values so later operations
 * receive a known state.
 */
void umi_reg_regulatory_service_init(UmiRegulatoryService *s){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL)memset(s,0,sizeof *s); }
/*
 * Provide the reg regulatory service set readiness operation used by this module and its
 * client applications.
 */
UmiStatus umi_reg_regulatory_service_set_readiness(UmiRegulatoryService *s,int t,int g,int c,int sub){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||(t!=0&&t!=1)||(g!=0&&g!=1)||(c!=0&&c!=1)||(sub!=0&&sub!=1))return UMI_STATUS_INVALID_ARGUMENT;s->taxonomy_ready=t;s->governance_ready=g;s->controls_ready=c;s->submission_ready=sub;return UMI_STATUS_OK; }
/*
 * Provide the reg regulatory service ready operation used by this module and its client
 * applications.
 */
int umi_reg_regulatory_service_ready(const UmiRegulatoryService *s){ return s!=NULL&&s->taxonomy_ready&&s->governance_ready&&s->controls_ready&&s->submission_ready&&s->registered_authorities>0U&&s->active_templates>0U?1:0; }
