/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/style_rule.c
 *
 * PURPOSE:
 *   Bind semantic component kind, intent and interaction state to a bounded set of style values.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/style_rule.h"

#include <string.h>
/*
 * Provide the design style rule upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_design_style_rule_upsert(UmiDesignStyleRule *rule,const UmiDesignStyleValue *value){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(rule==NULL||value==NULL||value->property_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<rule->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(rule->values[i].property_id,value->property_id)==0){rule->values[i]=*value;return UMI_STATUS_OK;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(rule->count>=UMI_DESIGN_MAX_VALUES)return UMI_STATUS_CAPACITY_EXCEEDED;rule->values[rule->count]=*value;++rule->count;return UMI_STATUS_OK;}
/*
 * Find design style rule while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_design_style_rule_find(const UmiDesignStyleRule *rule,const char *property_id,UmiDesignStyleValue *out_value){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(rule==NULL||property_id==NULL||out_value==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<rule->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(rule->values[i].property_id,property_id)==0){*out_value=rule->values[i];return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
