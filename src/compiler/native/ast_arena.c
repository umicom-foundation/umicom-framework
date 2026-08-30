/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/ast_arena.c
 *
 * PURPOSE:
 *   Allocate and index AST nodes in a bounded arena suitable for deterministic native compiler passes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/ast_arena.h"
#include <string.h>
void umi_nc_ast_arena_init(UmiNativeAstArena *a){ if(a!=NULL){memset(a,0,sizeof(*a));a->next_id=1U;} }
UmiStatus umi_nc_ast_arena_allocate(UmiNativeAstArena *a,UmiNativeAstKind kind,UmiNativeSourceSpan span,const char *text,uint32_t *out_id){ if(a==NULL||out_id==NULL) return UMI_STATUS_INVALID_ARGUMENT; if(a->count>=UMI_NC_MAX_AST_NODES) return UMI_STATUS_CAPACITY_EXCEEDED; uint32_t id=a->next_id++; UmiStatus st=umi_nc_ast_node_init(&a->nodes[a->count],id,kind,span,text); if(st!=UMI_STATUS_OK) return st; a->count++;a->revision++;*out_id=id;return UMI_STATUS_OK; }
UmiNativeAstNode *umi_nc_ast_arena_get(UmiNativeAstArena *a,uint32_t id){ if(a==NULL||id==0U) return NULL; for(size_t i=0U;i<a->count;i++) if(a->nodes[i].id==id) return &a->nodes[i]; return NULL; }
const UmiNativeAstNode *umi_nc_ast_arena_get_const(const UmiNativeAstArena *a,uint32_t id){ if(a==NULL||id==0U) return NULL; for(size_t i=0U;i<a->count;i++) if(a->nodes[i].id==id) return &a->nodes[i]; return NULL; }
