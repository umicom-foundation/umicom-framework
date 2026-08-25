/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/conformance_suite.c
 *
 * PURPOSE:
 *   bounded ordered collection of conformance rules for one application or workstation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/conformance_suite.h"

void umi_fc_conformance_suite_init(UmiFcConformanceSuite *suite,const char *id){if(suite!=NULL){*suite=(UmiFcConformanceSuite){0};if(id!=NULL)(void)umi_fc_copy_text(suite->id,sizeof(suite->id),id);}}
UmiStatus umi_fc_conformance_suite_add(UmiFcConformanceSuite *suite,const UmiFcConformanceRule *rule){if(suite==NULL||rule==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(suite->count>=UMI_FC_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;suite->rules[suite->count++]=*rule;return UMI_STATUS_OK;}
size_t umi_fc_conformance_suite_blocker_count(const UmiFcConformanceSuite *suite){size_t i,n=0U;if(suite==NULL)return 0U;for(i=0U;i<suite->count;++i)if(suite->rules[i].severity==UMI_FC_BLOCKER)n++;return n;}
