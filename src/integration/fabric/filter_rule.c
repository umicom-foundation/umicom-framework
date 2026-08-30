/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/filter_rule.c
 *
 * PURPOSE:
 *   Evaluate one simple filter against caller-supplied field/value evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/filter_rule.h"
#include <string.h>
#include <limits.h>


bool umi_fabric_filter_rule_match(const UmiFabricFilterExpression *expression,const char *field,const char *value){size_t n;if(expression==NULL||field==NULL||value==NULL||strcmp(expression->field,field)!=0)return false;if(strcmp(expression->operation,"eq")==0)return strcmp(expression->value,value)==0;if(strcmp(expression->operation,"prefix")==0){n=strlen(expression->value);return strncmp(expression->value,value,n)==0;}return false;}
