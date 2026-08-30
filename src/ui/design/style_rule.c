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
UmiStatus umi_design_style_rule_upsert(UmiDesignStyleRule *rule,const UmiDesignStyleValue *value){size_t i;if(rule==NULL||value==NULL||value->property_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<rule->count;++i)if(strcmp(rule->values[i].property_id,value->property_id)==0){rule->values[i]=*value;return UMI_STATUS_OK;}if(rule->count>=UMI_DESIGN_MAX_VALUES)return UMI_STATUS_CAPACITY_EXCEEDED;rule->values[rule->count]=*value;++rule->count;return UMI_STATUS_OK;}
UmiStatus umi_design_style_rule_find(const UmiDesignStyleRule *rule,const char *property_id,UmiDesignStyleValue *out_value){size_t i;if(rule==NULL||property_id==NULL||out_value==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<rule->count;++i)if(strcmp(rule->values[i].property_id,property_id)==0){*out_value=rule->values[i];return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
