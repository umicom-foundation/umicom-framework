/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/environment_profile.c
 *
 * PURPOSE:
 *   bounded environment-variable overlay for packaged applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/environment_profile.h"

#include <string.h>
/*
 * Initialise dr environment profile from caller-provided values so later operations
 * receive a known state.
 */
void umi_dr_environment_profile_init(UmiDrEnvironmentProfile*p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p!=NULL)*p=(UmiDrEnvironmentProfile){0};}
/*
 * Provide the dr environment profile get operation used by this module and its client
 * applications.
 */
const char *umi_dr_environment_profile_get(const UmiDrEnvironmentProfile*p,const char*k){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||k==NULL)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<p->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(p->entries[i].key,k)==0)return p->entries[i].value;return NULL;}
/*
 * Copy dr environment profile into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_dr_environment_profile_set(UmiDrEnvironmentProfile*p,const char*k,const char*v){size_t i;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||k==NULL||v==NULL||k[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<p->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(p->entries[i].key,k)==0)return umi_dr_copy_text(p->entries[i].value,sizeof(p->entries[i].value),v);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->count>=UMI_DR_MAX_ENV)return UMI_STATUS_CAPACITY_EXCEEDED;s=umi_dr_copy_text(p->entries[p->count].key,sizeof(p->entries[p->count].key),k);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_dr_copy_text(p->entries[p->count].value,sizeof(p->entries[p->count].value),v);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)p->count++;return s;}
