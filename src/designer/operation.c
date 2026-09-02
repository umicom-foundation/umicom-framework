/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/operation.c
 *
 * PURPOSE:
 *   Apply and reverse designer mutations against the authoritative declarative document without direct widget state changes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The functions below modify semantic designer state so undo, preview and
 * generation behave the same in Studio, headless tests and future hosts.
 */

#include "umicom/designer/operation.h"
#include <string.h>
/* Provide the init operation used by this module and its client applications. */
static UmiStatus init(UmiDesignerOperation *op,UmiDesignerOperationKind kind,const UmiDeclNode *before,const UmiDeclNode *after,const char *property){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(op==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(op,0,sizeof(*op));op->kind=kind;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(before!=NULL){op->node_before=*before;op->has_before=1;(void)umi_decl_copy_text(op->node_id,sizeof(op->node_id),before->node_id);}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(after!=NULL){op->node_after=*after;op->has_after=1;(void)umi_decl_copy_text(op->node_id,sizeof(op->node_id),after->node_id);}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(property!=NULL)(void)umi_decl_copy_text(op->property_name,sizeof(op->property_name),property);return UMI_STATUS_OK;}
/* Add designer operation only after its inputs and available capacity have been checked. */
UmiStatus umi_designer_operation_add(const UmiDeclNode *node,UmiDesignerOperation *out){return node!=NULL?init(out,UMI_DESIGNER_OP_ADD_COMPONENT,NULL,node,NULL):UMI_STATUS_INVALID_ARGUMENT;}
/*
 * Remove designer operation while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_designer_operation_remove(const UmiDeclNode *node,UmiDesignerOperation *out){return node!=NULL?init(out,UMI_DESIGNER_OP_REMOVE_COMPONENT,node,NULL,NULL):UMI_STATUS_INVALID_ARGUMENT;}
/*
 * Provide the designer operation set property operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_operation_set_property(const UmiDeclNode *before,const UmiDeclNode *after,const char *name,UmiDesignerOperation *out){return before!=NULL&&after!=NULL&&name!=NULL?init(out,UMI_DESIGNER_OP_SET_PROPERTY,before,after,name):UMI_STATUS_INVALID_ARGUMENT;}
/*
 * Provide the designer operation move operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_operation_move(const UmiDeclNode *before,const UmiDeclNode *after,UmiDesignerOperation *out){return before!=NULL&&after!=NULL?init(out,UMI_DESIGNER_OP_MOVE_COMPONENT,before,after,NULL):UMI_STATUS_INVALID_ARGUMENT;}
/*
 * Perform designer operation through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_designer_operation_apply(UmiDesignerDocument *designer,const UmiDesignerOperation *op,int reverse){UmiDeclDocument *doc;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(designer==NULL||op==NULL)return UMI_STATUS_INVALID_ARGUMENT;doc=umi_designer_document_declarative(designer);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(op->kind==UMI_DESIGNER_OP_ADD_COMPONENT)s=reverse?umi_decl_document_remove_node(doc,op->node_id):umi_decl_document_add_node(doc,&op->node_after);else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(op->kind==UMI_DESIGNER_OP_REMOVE_COMPONENT)s=reverse?umi_decl_document_add_node(doc,&op->node_before):umi_decl_document_remove_node(doc,op->node_id);/* Use this fallback path when the earlier condition does not apply. */ else{s=umi_decl_document_update_node(doc,reverse?&op->node_before:&op->node_after);}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)umi_designer_document_mark_changed(designer);return s;}
