/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_ast_arena.c
 *
 * PURPOSE:
 *   Regression coverage for allocate and index ast nodes in a bounded arena suitable for deterministic native compiler passes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/ast_arena.h"
int main(void){ UmiNativeAstArena a; umi_nc_ast_arena_init(&a); uint32_t id=0U; UmiNativeSourceSpan s={0}; if(umi_nc_ast_arena_allocate(&a,UMI_NC_AST_LITERAL,s,"1",&id)!=UMI_STATUS_OK) return 1; if(id!=1U||umi_nc_ast_arena_get(&a,id)==NULL||a.revision!=1U) return 2; return 0; }
