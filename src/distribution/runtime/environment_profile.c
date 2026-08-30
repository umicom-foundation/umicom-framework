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
void umi_dr_environment_profile_init(UmiDrEnvironmentProfile*p){if(p!=NULL)*p=(UmiDrEnvironmentProfile){0};}
const char *umi_dr_environment_profile_get(const UmiDrEnvironmentProfile*p,const char*k){size_t i;if(p==NULL||k==NULL)return NULL;for(i=0U;i<p->count;++i)if(strcmp(p->entries[i].key,k)==0)return p->entries[i].value;return NULL;}
UmiStatus umi_dr_environment_profile_set(UmiDrEnvironmentProfile*p,const char*k,const char*v){size_t i;UmiStatus s;if(p==NULL||k==NULL||v==NULL||k[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<p->count;++i)if(strcmp(p->entries[i].key,k)==0)return umi_dr_copy_text(p->entries[i].value,sizeof(p->entries[i].value),v);if(p->count>=UMI_DR_MAX_ENV)return UMI_STATUS_CAPACITY_EXCEEDED;s=umi_dr_copy_text(p->entries[p->count].key,sizeof(p->entries[p->count].key),k);if(s!=UMI_STATUS_OK)return s;s=umi_dr_copy_text(p->entries[p->count].value,sizeof(p->entries[p->count].value),v);if(s==UMI_STATUS_OK)p->count++;return s;}
