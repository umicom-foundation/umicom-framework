/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_ast_node.c
 *
 * PURPOSE:
 *   Regression coverage for represent bounded semantic ast nodes with stable child links and source spans.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/ast_node.h"
int main(void){ UmiNativeAstNode n; UmiNativeSourceSpan s={0}; if(umi_nc_ast_node_init(&n,1U,UMI_NC_AST_BINARY,s,"+")!=UMI_STATUS_OK) return 1; if(umi_nc_ast_node_add_child(&n,2U)!=UMI_STATUS_OK||umi_nc_ast_node_add_child(&n,3U)!=UMI_STATUS_OK) return 2; if(!umi_nc_ast_node_is_expression(&n)||n.child_count!=2U) return 3; return 0; }
