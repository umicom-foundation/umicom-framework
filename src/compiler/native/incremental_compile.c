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
UmiStatus umi_nc_incremental_unit_init(UmiNativeIncrementalUnit *u,const char *path,uint64_t s,uint64_t o,uint64_t d){if(u==NULL||path==NULL||path[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;memset(u,0,sizeof(*u));if(umi_nc_copy_text(u->path,sizeof(u->path),path)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;u->source_hash=s;u->options_hash=o;u->dependency_hash=d;return UMI_STATUS_OK;}
bool umi_nc_incremental_needs_rebuild(const UmiNativeIncrementalUnit *p,const UmiNativeIncrementalUnit *c){if(p==NULL||c==NULL)return true;if(!p->artifact_present)return true;if(strcmp(p->path,c->path)!=0)return true;return p->source_hash!=c->source_hash||p->options_hash!=c->options_hash||p->dependency_hash!=c->dependency_hash;}
