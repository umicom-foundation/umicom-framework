/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/conformance_suite.c
 *
 * PURPOSE:
 *   bounded ordered collection of conformance rules for one application or workstation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/conformance_suite.h"

/*
 * Initialise fc conformance suite from caller-provided values so later operations receive
 * a known state.
 */
void umi_fc_conformance_suite_init(UmiFcConformanceSuite *suite,const char *id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(suite!=NULL){*suite=(UmiFcConformanceSuite){0};/* Protect caller-owned memory by checking that required state is available before it is used. */ if(id!=NULL)(void)umi_fc_copy_text(suite->id,sizeof(suite->id),id);}}
/* Add fc conformance suite only after its inputs and available capacity have been checked. */
UmiStatus umi_fc_conformance_suite_add(UmiFcConformanceSuite *suite,const UmiFcConformanceRule *rule){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(suite==NULL||rule==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(suite->count>=UMI_FC_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;suite->rules[suite->count++]=*rule;return UMI_STATUS_OK;}
/*
 * Return the number of records represented by fc conformance suite blocker without
 * changing their state.
 */
size_t umi_fc_conformance_suite_blocker_count(const UmiFcConformanceSuite *suite){size_t i,n=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(suite==NULL)return 0U;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<suite->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(suite->rules[i].severity==UMI_FC_BLOCKER)n++;return n;}
