/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/risk_weight_table.c
 *
 * PURPOSE:
 *   Store and resolve ordered risk-weight rules by exposure-class identifier.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/risk_weight_table.h"

#include <string.h>
/*
 * Add pru risk weight table only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_pru_risk_weight_table_add(UmiPrudentialRiskWeightTable *table,const UmiPrudentialRiskWeightRule *rule) { size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(table==NULL||rule==NULL)return UMI_STATUS_INVALID_ARGUMENT; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<table->count;++i) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(table->rules[i].exposure_class_id,rule->exposure_class_id)==0)return UMI_STATUS_ALREADY_EXISTS; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(table->count>=UMI_PRU_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED; table->rules[table->count++]=*rule; return UMI_STATUS_OK; }
/*
 * Find pru risk weight table while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_pru_risk_weight_table_find(const UmiPrudentialRiskWeightTable *table,const char *exposure_class_id,double *out_weight) { size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(table==NULL||exposure_class_id==NULL||out_weight==NULL)return UMI_STATUS_INVALID_ARGUMENT; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<table->count;++i) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(table->rules[i].exposure_class_id,exposure_class_id)==0){*out_weight=table->rules[i].risk_weight;return UMI_STATUS_OK;} return UMI_STATUS_NOT_FOUND; }
