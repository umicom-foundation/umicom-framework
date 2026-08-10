/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/operation.h
 *
 * PURPOSE:
 *   Represent one undoable visual-designer mutation with enough before/after state for deterministic reversal.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract describes reusable visual-designer state and operations.
 * Product-specific windows remain outside the Framework engine.
 */

#ifndef UMICOM_DESIGNER_OPERATION_H
#define UMICOM_DESIGNER_OPERATION_H
#include "umicom/designer/document.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDesignerOperation { UmiDesignerOperationKind kind; char node_id[UMI_DECL_ID_CAPACITY]; char parent_before[UMI_DECL_ID_CAPACITY]; char parent_after[UMI_DECL_ID_CAPACITY]; char property_name[UMI_DECL_NAME_CAPACITY]; UmiDeclNode node_before; UmiDeclNode node_after; int has_before; int has_after; } UmiDesignerOperation;
UmiStatus umi_designer_operation_add(const UmiDeclNode *node,UmiDesignerOperation *out_operation);
UmiStatus umi_designer_operation_remove(const UmiDeclNode *node,UmiDesignerOperation *out_operation);
UmiStatus umi_designer_operation_set_property(const UmiDeclNode *before,const UmiDeclNode *after,const char *property_name,UmiDesignerOperation *out_operation);
UmiStatus umi_designer_operation_move(const UmiDeclNode *before,const UmiDeclNode *after,UmiDesignerOperation *out_operation);
UmiStatus umi_designer_operation_apply(UmiDesignerDocument *document,const UmiDesignerOperation *operation,int reverse);
#ifdef __cplusplus
}
#endif
#endif
