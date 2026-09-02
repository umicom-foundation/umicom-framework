/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/incremental_compile.c
 *
 * PURPOSE:
 *   Decide translation-unit rebuilds from source, options, dependency and artifact fingerprints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/incremental_compile.h"
#include <string.h>
/*
 * Initialise nc incremental unit from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_nc_incremental_unit_init(UmiNativeIncrementalUnit *u,const char *path,uint64_t s,uint64_t o,uint64_t d){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(u==NULL||path==NULL||path[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;memset(u,0,sizeof(*u));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_copy_text(u->path,sizeof(u->path),path)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;u->source_hash=s;u->options_hash=o;u->dependency_hash=d;return UMI_STATUS_OK;}
/*
 * Provide the nc incremental needs rebuild operation used by this module and its client
 * applications.
 */
bool umi_nc_incremental_needs_rebuild(const UmiNativeIncrementalUnit *p,const UmiNativeIncrementalUnit *c){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||c==NULL)return true;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!p->artifact_present)return true;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(p->path,c->path)!=0)return true;return p->source_hash!=c->source_hash||p->options_hash!=c->options_hash||p->dependency_hash!=c->dependency_hash;}
