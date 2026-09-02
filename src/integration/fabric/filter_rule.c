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


/*
 * Provide the fabric filter rule match operation used by this module and its client
 * applications.
 */
bool umi_fabric_filter_rule_match(const UmiFabricFilterExpression *expression,const char *field,const char *value){size_t n;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(expression==NULL||field==NULL||value==NULL||strcmp(expression->field,field)!=0)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(expression->operation,"eq")==0)return strcmp(expression->value,value)==0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(expression->operation,"prefix")==0){n=strlen(expression->value);return strncmp(expression->value,value,n)==0;}return false;}
