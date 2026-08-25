/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/ast_node.c
 *
 * PURPOSE:
 *   Represent bounded semantic AST nodes with stable child links and source spans.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/ast_node.h"
#include <string.h>
UmiStatus umi_nc_ast_node_init(UmiNativeAstNode *n,uint32_t id,UmiNativeAstKind kind,UmiNativeSourceSpan span,const char *text){ if(n==NULL||id==0U||kind==UMI_NC_AST_INVALID) return UMI_STATUS_INVALID_ARGUMENT; memset(n,0,sizeof(*n));n->id=id;n->kind=kind;n->span=span; if(text!=NULL&&umi_nc_copy_text(n->text,sizeof(n->text),text)!=UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED; return UMI_STATUS_OK; }
UmiStatus umi_nc_ast_node_add_child(UmiNativeAstNode *n,uint32_t child_id){ if(n==NULL||child_id==0U) return UMI_STATUS_INVALID_ARGUMENT; if(n->child_count>=UMI_NC_MAX_AST_CHILDREN) return UMI_STATUS_CAPACITY_EXCEEDED; n->child_ids[n->child_count++]=child_id; return UMI_STATUS_OK; }
bool umi_nc_ast_node_is_expression(const UmiNativeAstNode *n){ if(n==NULL)return false; return n->kind==UMI_NC_AST_BINARY||n->kind==UMI_NC_AST_UNARY||n->kind==UMI_NC_AST_CALL||n->kind==UMI_NC_AST_IDENTIFIER||n->kind==UMI_NC_AST_LITERAL; }
