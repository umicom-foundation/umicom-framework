/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/conformance_rule.c
 *
 * PURPOSE:
 *   weighted conformance rules with severity and required capability evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/conformance_rule.h"

/*
 * Provide the fc conformance rule make operation used by this module and its client
 * applications.
 */
UmiStatus umi_fc_conformance_rule_make(const char *id,UmiFcSeverity severity,uint64_t required_capabilities,double weight,UmiFcConformanceRule *out_rule){UmiStatus st;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_rule==NULL||weight<0.0)return UMI_STATUS_INVALID_ARGUMENT;*out_rule=(UmiFcConformanceRule){0};st=umi_fc_copy_text(out_rule->id,sizeof(out_rule->id),id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st;out_rule->severity=severity;out_rule->required_capabilities=required_capabilities;out_rule->weight=weight;return UMI_STATUS_OK;}
/*
 * Provide the fc conformance rule passes operation used by this module and its client
 * applications.
 */
bool umi_fc_conformance_rule_passes(const UmiFcConformanceRule *rule,uint64_t actual){return rule!=NULL&&(actual&rule->required_capabilities)==rule->required_capabilities;}
