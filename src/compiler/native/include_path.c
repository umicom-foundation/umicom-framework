/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/include_path.c
 *
 * PURPOSE:
 *   Manage portable user/system include roots and construct normalized include candidates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/include_path.h"
#include <string.h>
/*
 * Initialise nc include paths from caller-provided values so later operations receive a
 * known state.
 */
void umi_nc_include_paths_init(UmiNativeIncludePathSet *p){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p!=NULL) memset(p,0,sizeof(*p)); }
/* Add nc include paths only after its inputs and available capacity have been checked. */
UmiStatus umi_nc_include_paths_add(UmiNativeIncludePathSet *p,const char *path,bool system){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||path==NULL||path[0]=='\0') return UMI_STATUS_INVALID_ARGUMENT; /* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<p->count;i++) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(p->items[i].path,path)==0){p->items[i].system=system;return UMI_STATUS_OK;} /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->count>=UMI_NC_MAX_INCLUDE_PATHS) return UMI_STATUS_CAPACITY_EXCEEDED; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_copy_text(p->items[p->count].path,sizeof(p->items[p->count].path),path)!=UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED; p->items[p->count].system=system;p->count++;p->revision++;return UMI_STATUS_OK; }
/*
 * Provide the nc include candidate operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_include_candidate(const UmiNativeIncludeRoot *root,const char *header,char *out,size_t cap){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(root==NULL||header==NULL||out==NULL||cap==0U) return UMI_STATUS_INVALID_ARGUMENT; size_t a=strlen(root->path),b=strlen(header); bool sep=a>0U&&root->path[a-1U]!='/'&&root->path[a-1U]!='\\'; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(a+(sep?1U:0U)+b+1U>cap) return UMI_STATUS_CAPACITY_EXCEEDED; memcpy(out,root->path,a); size_t n=a; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(sep) out[n++]='/'; memcpy(out+n,header,b+1U); return UMI_STATUS_OK; }
