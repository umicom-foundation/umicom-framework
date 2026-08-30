/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/ast_node.h
 *
 * PURPOSE:
 *   Represent bounded semantic AST nodes with stable child links and source spans.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_AST_NODE_H
#define UMICOM_COMPILER_NATIVE_AST_NODE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_NC_MAX_AST_CHILDREN 8U
typedef struct UmiNativeAstNode { uint32_t id; UmiNativeAstKind kind; UmiNativeSourceSpan span; uint32_t child_ids[UMI_NC_MAX_AST_CHILDREN]; size_t child_count; uint32_t type_id; uint32_t symbol_id; int64_t integer_payload; char text[UMI_NC_NAME_CAPACITY]; } UmiNativeAstNode;
UmiStatus umi_nc_ast_node_init(UmiNativeAstNode *node,uint32_t id,UmiNativeAstKind kind,UmiNativeSourceSpan span,const char *text);
UmiStatus umi_nc_ast_node_add_child(UmiNativeAstNode *node,uint32_t child_id);
bool umi_nc_ast_node_is_expression(const UmiNativeAstNode *node);
#ifdef __cplusplus
}
#endif
#endif
