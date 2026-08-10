/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_designer_history.c
 *
 * PURPOSE:
 *   Verify semantic designer changes can be undone and redone without manipulating widgets.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This test exercises the public contract rather than relying on private state,
 * making the behaviour useful as an executable example for new developers.
 */
#include "umicom/umicom.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    UmiDesignerDocument *document=NULL;UmiDesignerHistory *history=NULL;UmiDeclNode root;UmiDesignerOperation operation;
    assert(umi_designer_document_create("org.umicom.demo",&document)==UMI_STATUS_OK);
    assert(umi_designer_history_create(document,&history)==UMI_STATUS_OK);
    assert(umi_decl_node_init(&root,"root","window","-")==UMI_STATUS_OK);
    assert(umi_designer_operation_add(&root,&operation)==UMI_STATUS_OK);
    assert(umi_designer_history_execute(history,&operation)==UMI_STATUS_OK);
    assert(umi_decl_document_node_count(umi_designer_document_declarative(document))==1U);
    assert(umi_designer_history_undo(history)==UMI_STATUS_OK);
    assert(umi_decl_document_node_count(umi_designer_document_declarative(document))==0U);
    assert(umi_designer_history_redo(history)==UMI_STATUS_OK);
    umi_designer_history_destroy(history);umi_designer_document_destroy(document);
    return EXIT_SUCCESS;
}
