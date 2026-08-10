/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/operation.c
 *
 * PURPOSE:
 *   Apply and reverse designer mutations against the authoritative declarative document without direct widget state changes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The functions below modify semantic designer state so undo, preview and
 * generation behave the same in Studio, headless tests and future hosts.
 */

#include "umicom/designer/operation.h"
#include <string.h>
static UmiStatus init(UmiDesignerOperation *op,UmiDesignerOperationKind kind,const UmiDeclNode *before,const UmiDeclNode *after,const char *property){if(op==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(op,0,sizeof(*op));op->kind=kind;if(before!=NULL){op->node_before=*before;op->has_before=1;(void)umi_decl_copy_text(op->node_id,sizeof(op->node_id),before->node_id);}if(after!=NULL){op->node_after=*after;op->has_after=1;(void)umi_decl_copy_text(op->node_id,sizeof(op->node_id),after->node_id);}if(property!=NULL)(void)umi_decl_copy_text(op->property_name,sizeof(op->property_name),property);return UMI_STATUS_OK;}
UmiStatus umi_designer_operation_add(const UmiDeclNode *node,UmiDesignerOperation *out){return node!=NULL?init(out,UMI_DESIGNER_OP_ADD_COMPONENT,NULL,node,NULL):UMI_STATUS_INVALID_ARGUMENT;}
UmiStatus umi_designer_operation_remove(const UmiDeclNode *node,UmiDesignerOperation *out){return node!=NULL?init(out,UMI_DESIGNER_OP_REMOVE_COMPONENT,node,NULL,NULL):UMI_STATUS_INVALID_ARGUMENT;}
UmiStatus umi_designer_operation_set_property(const UmiDeclNode *before,const UmiDeclNode *after,const char *name,UmiDesignerOperation *out){return before!=NULL&&after!=NULL&&name!=NULL?init(out,UMI_DESIGNER_OP_SET_PROPERTY,before,after,name):UMI_STATUS_INVALID_ARGUMENT;}
UmiStatus umi_designer_operation_move(const UmiDeclNode *before,const UmiDeclNode *after,UmiDesignerOperation *out){return before!=NULL&&after!=NULL?init(out,UMI_DESIGNER_OP_MOVE_COMPONENT,before,after,NULL):UMI_STATUS_INVALID_ARGUMENT;}
UmiStatus umi_designer_operation_apply(UmiDesignerDocument *designer,const UmiDesignerOperation *op,int reverse){UmiDeclDocument *doc;UmiStatus s;if(designer==NULL||op==NULL)return UMI_STATUS_INVALID_ARGUMENT;doc=umi_designer_document_declarative(designer);if(op->kind==UMI_DESIGNER_OP_ADD_COMPONENT)s=reverse?umi_decl_document_remove_node(doc,op->node_id):umi_decl_document_add_node(doc,&op->node_after);else if(op->kind==UMI_DESIGNER_OP_REMOVE_COMPONENT)s=reverse?umi_decl_document_add_node(doc,&op->node_before):umi_decl_document_remove_node(doc,op->node_id);else{s=umi_decl_document_update_node(doc,reverse?&op->node_before:&op->node_after);}if(s==UMI_STATUS_OK)umi_designer_document_mark_changed(designer);return s;}
