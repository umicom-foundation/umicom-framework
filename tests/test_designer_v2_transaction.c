/*-----------------------------------------------------------------------------
 * Umicom Framework - Visual Application Builder v2 transaction tests
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Two operations become one undoable user action. */
#include "umicom/umicom.h"
#include <assert.h>
#include <stdlib.h>
int main(void)
{
    UmiDesignerDocument *document=NULL; UmiDesignerTransactionHistoryV2 *history=NULL; UmiDesignerTransactionV2 tx; UmiDeclNode a,b; UmiDesignerOperation operation;
    assert(umi_designer_document_create("org.umicom.tx",&document)==UMI_STATUS_OK);
    assert(umi_designer_transaction_history_v2_create(document,&history)==UMI_STATUS_OK);
    assert(umi_designer_transaction_v2_init(&tx,"add-pair","Add pair")==UMI_STATUS_OK);
    assert(umi_decl_node_init(&a,"a","pane","-")==UMI_STATUS_OK); assert(umi_designer_operation_add(&a,&operation)==UMI_STATUS_OK); assert(umi_designer_transaction_v2_add(&tx,&operation)==UMI_STATUS_OK);
    assert(umi_decl_node_init(&b,"b","button","a")==UMI_STATUS_OK); assert(umi_designer_operation_add(&b,&operation)==UMI_STATUS_OK); assert(umi_designer_transaction_v2_add(&tx,&operation)==UMI_STATUS_OK);
    assert(umi_designer_transaction_history_v2_execute(history,&tx)==UMI_STATUS_OK); assert(umi_decl_document_node_count(umi_designer_document_declarative(document))==2U);
    assert(umi_designer_transaction_history_v2_undo(history)==UMI_STATUS_OK); assert(umi_decl_document_node_count(umi_designer_document_declarative(document))==0U);
    assert(umi_designer_transaction_history_v2_redo(history)==UMI_STATUS_OK); assert(umi_decl_document_node_count(umi_designer_document_declarative(document))==2U);
    umi_designer_transaction_history_v2_destroy(history); umi_designer_document_destroy(document); return EXIT_SUCCESS;
}
