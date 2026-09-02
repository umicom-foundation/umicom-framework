/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/cross_field_validation.c
 *
 * PURPOSE:
 *   Evaluate deterministic cross-field equality and tolerance constraints.
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

#include "umicom/finance/regulatory/cross_field_validation.h"

#include <math.h>
#include <string.h>
/*
 * Initialise reg cross field validation from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_reg_cross_field_validation_init(UmiCrossFieldValidation *rule,const char *rule_id,double tolerance){ UmiStatus s; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(rule==NULL||!umi_reg_number_valid(tolerance)||tolerance<0.0)return UMI_STATUS_INVALID_ARGUMENT; memset(rule,0,sizeof *rule); s=umi_reg_copy_text(rule->rule_id,sizeof rule->rule_id,rule_id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)rule->tolerance=tolerance; return s; }
/*
 * Provide the reg cross field validation passes operation used by this module and its
 * client applications.
 */
int umi_reg_cross_field_validation_passes(const UmiCrossFieldValidation *rule,double left,double right){ return rule!=NULL&&umi_reg_number_valid(left)&&umi_reg_number_valid(right)&&fabs(left-right)<=rule->tolerance?1:0; }
