/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/service.c
 *
 * PURPOSE:
 *   Keep a reusable scan root and retained result for Studio, Helix and CI
 *   hosts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */

#include "umicom/codeguard/service.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct UmiCodeGuardService { char root[1024]; UmiCodeGuardResult *result; };
/*
 * Initialise codeguard service from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_codeguard_service_create(const char *root,UmiCodeGuardService **out){UmiCodeGuardService *s;UmiStatus st;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(root==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;s=(UmiCodeGuardService*)calloc(1U,sizeof(*s));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_OUT_OF_MEMORY;(void)snprintf(s->root,sizeof(s->root),"%s",root);st=umi_codeguard_result_create(256U,&s->result);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK){free(s);return st;}*out=s;return UMI_STATUS_OK;}
/*
 * Release or reset state held by codeguard service so the same storage can be reused
 * safely.
 */
void umi_codeguard_service_destroy(UmiCodeGuardService *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL){umi_codeguard_result_destroy(s->result);free(s);}}
/*
 * Provide the codeguard service scan operation used by this module and its client
 * applications.
 */
UmiStatus umi_codeguard_service_scan(UmiCodeGuardService *s,const UmiCodeGuardProfile *p){UmiCodeGuardConfig c;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;umi_codeguard_result_clear(s->result);c=umi_codeguard_config_default(s->root);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p!=NULL)c.profile=*p;return umi_codeguard_scan(&c,s->result);}
/*
 * Provide the codeguard service result operation used by this module and its client
 * applications.
 */
const UmiCodeGuardResult *umi_codeguard_service_result(const UmiCodeGuardService *s){return s!=NULL?s->result:NULL;}
/*
 * Provide the codeguard service summary operation used by this module and its client
 * applications.
 */
UmiCodeGuardSummary umi_codeguard_service_summary(const UmiCodeGuardService *s){return umi_codeguard_summary_build(s!=NULL?s->result:NULL);}
