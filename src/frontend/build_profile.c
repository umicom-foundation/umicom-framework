/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/build_profile.c
 *
 * PURPOSE:
 *   Implement the build profile behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/frontend/build_profile.h"
#include <string.h>
/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiFrontendBuildProfiles *p,const char *id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||id==NULL)return SIZE_MAX;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<p->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(p->items[i].id,id)==0)return i;return SIZE_MAX;}
/*
 * Initialise frontend build profiles from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_frontend_build_profiles_init(UmiFrontendBuildProfiles *p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(p,0,sizeof(*p));p->revision=1U;return UMI_STATUS_OK;}
/*
 * Add frontend build profiles only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_frontend_build_profiles_add(UmiFrontendBuildProfiles *p,const UmiFrontendBuildProfile *v){size_t i;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||v==NULL||v->id[0]=='\0'||v->entry_file[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;i=find_index(p,v->id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->count>=UMI_FRONTEND_DEV_MAX_BUILD_PROFILES)return UMI_STATUS_CAPACITY_EXCEEDED;i=p->count++;}p->items[i]=*v;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->active_id[0]=='\0'){s=umi_frontend_dev_copy_text(p->active_id,sizeof(p->active_id),v->id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;}p->revision+=1U;return UMI_STATUS_OK;}
/*
 * Provide the frontend build profiles activate operation used by this module and its
 * client applications.
 */
UmiStatus umi_frontend_build_profiles_activate(UmiFrontendBuildProfiles *p,const char *id){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(find_index(p,id)==SIZE_MAX)return UMI_STATUS_NOT_FOUND;s=umi_frontend_dev_copy_text(p->active_id,sizeof(p->active_id),id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)p->revision+=1U;return s;}
/*
 * Provide the frontend build profiles active operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_build_profiles_active(const UmiFrontendBuildProfiles *p,UmiFrontendBuildProfile *out){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;i=find_index(p,p->active_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX)return UMI_STATUS_NOT_FOUND;*out=p->items[i];return UMI_STATUS_OK;}
