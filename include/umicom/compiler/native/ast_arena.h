/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/ast_arena.h
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
#ifndef UMICOM_COMPILER_NATIVE_AST_ARENA_H
#define UMICOM_COMPILER_NATIVE_AST_ARENA_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/ast_node.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the native ast arena data shared with callers of this public contract.
 */
typedef struct UmiNativeAstArena { UmiNativeAstNode nodes[UMI_NC_MAX_AST_NODES]; size_t count; uint32_t next_id; uint64_t revision; } UmiNativeAstArena;
/**
 * Initialise nc ast arena from caller-provided values so later operations receive a known
 * state.
 */
void umi_nc_ast_arena_init(UmiNativeAstArena *arena);
/**
 * Provide the nc ast arena allocate operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_ast_arena_allocate(UmiNativeAstArena *arena,UmiNativeAstKind kind,UmiNativeSourceSpan span,const char *text,uint32_t *out_id);
/**
 * Provide the nc ast arena get operation used by this module and its client applications.
 */
UmiNativeAstNode *umi_nc_ast_arena_get(UmiNativeAstArena *arena,uint32_t id);
/**
 * Provide the nc ast arena get const operation used by this module and its client
 * applications.
 */
const UmiNativeAstNode *umi_nc_ast_arena_get_const(const UmiNativeAstArena *arena,uint32_t id);
#ifdef __cplusplus
}
#endif
#endif
