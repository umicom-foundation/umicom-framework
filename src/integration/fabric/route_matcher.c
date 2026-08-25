/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/route_matcher.c
 *
 * PURPOSE:
 *   Match exact or suffix-wildcard patterns and choose the highest-priority enabled route.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/route_matcher.h"
#include <string.h>
#include <limits.h>


bool umi_fabric_route_pattern_match(const char *pattern,const char *value){size_t n;if(pattern==NULL||value==NULL)return false;if(strcmp(pattern,"*")==0)return true;n=strlen(pattern);if(n>0U&&pattern[n-1U]=='*')return strncmp(pattern,value,n-1U)==0;return strcmp(pattern,value)==0;}
UmiStatus umi_fabric_route_match(const UmiFabricRouteTable *table,const char *source,const char *message_type,UmiFabricRouteRule *out_rule){size_t i;bool found=false;uint32_t best=0U;if(table==NULL||source==NULL||message_type==NULL||out_rule==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<table->count;++i){const UmiFabricRouteRule *r=&table->items[i];if(!r->enabled||!umi_fabric_route_pattern_match(r->source_pattern,source)||!umi_fabric_route_pattern_match(r->message_pattern,message_type))continue;if(!found||r->priority>best||(r->priority==best&&strcmp(r->route_id,out_rule->route_id)<0)){*out_rule=*r;best=r->priority;found=true;}}return found?UMI_STATUS_OK:UMI_STATUS_NOT_FOUND;}
