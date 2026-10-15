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
#include "../../base/value_construction_internal.h"
#include <string.h>
/*
 * Initialise nc incremental unit from caller-provided values so later operations receive a
 * known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_nc_incremental_unit_init(UmiNativeIncrementalUnit *u,const char *path,uint64_t s,uint64_t o,uint64_t d){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(u==NULL||path==NULL||path[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;memset(u,0,sizeof(*u));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_copy_text(u->path,sizeof(u->path),path)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;u->source_hash=s;u->options_hash=o;u->dependency_hash=d;return UMI_STATUS_OK;}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_nc_incremental_unit_init_build_value(UmiNativeIncrementalUnit *u,const char *path,uint64_t s,uint64_t o,uint64_t d){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(u==NULL||path==NULL||path[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;memset(u,0,sizeof(*u));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_copy_text(u->path,sizeof(u->path),path)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;u->source_hash=s;u->options_hash=o;u->dependency_hash=d;return UMI_STATUS_OK;}

UMI_DEFINE_STAGED_VALUE_INIT(umi_nc_incremental_unit_init, UmiNativeIncrementalUnit, u,
    (UmiNativeIncrementalUnit *u, const char *path, uint64_t s, uint64_t o, uint64_t d),
    umi_nc_incremental_unit_init_build_value(candidate, path, s, o, d))

/*
 * Provide the nc incremental needs rebuild operation used by this module and its client
 * applications.
 */
bool umi_nc_incremental_needs_rebuild(const UmiNativeIncrementalUnit *p,const UmiNativeIncrementalUnit *c){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||c==NULL)return true;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!p->artifact_present)return true;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(p->path,c->path)!=0)return true;return p->source_hash!=c->source_hash||p->options_hash!=c->options_hash||p->dependency_hash!=c->dependency_hash;}
