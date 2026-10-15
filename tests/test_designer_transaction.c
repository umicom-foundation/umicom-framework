/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_designer_transaction.c
 *
 * PURPOSE:
 *   Implement the test designer transaction behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework - Visual Application Builder v2 transaction tests
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Two operations become one undoable user action. */
#include "umicom/umicom.h"
#include "value_archive/construction_cases.h"
#include <assert.h>
#include <stdlib.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted transaction value. */
static int UmiDesignerTransactionConstructionCases(void)
{
    UmiDesignerTransaction *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_designer_transaction_init(value, "add-pair", "Add pair") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->transaction_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_designer_transaction_init(value, oversized, "Add pair") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_designer_transaction_init(value, value->transaction_id, "Add pair") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->transaction_id, "add-pair") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void)
{
    if (UmiDesignerTransactionConstructionCases() != 0) return 1;

    UmiDesignerDocument *document=NULL; UmiDesignerTransactionHistory *history=NULL; UmiDesignerTransaction tx; UmiDeclNode a,b; UmiDesignerOperation operation;
    assert(umi_designer_document_create("org.umicom.tx",&document)==UMI_STATUS_OK);
    assert(umi_designer_transaction_history_create(document,&history)==UMI_STATUS_OK);
    assert(umi_designer_transaction_init(&tx,"add-pair","Add pair")==UMI_STATUS_OK);
    assert(umi_decl_node_init(&a,"a","pane","-")==UMI_STATUS_OK); assert(umi_designer_operation_add(&a,&operation)==UMI_STATUS_OK); assert(umi_designer_transaction_add(&tx,&operation)==UMI_STATUS_OK);
    assert(umi_decl_node_init(&b,"b","button","a")==UMI_STATUS_OK); assert(umi_designer_operation_add(&b,&operation)==UMI_STATUS_OK); assert(umi_designer_transaction_add(&tx,&operation)==UMI_STATUS_OK);
    assert(umi_designer_transaction_history_execute(history,&tx)==UMI_STATUS_OK); assert(umi_decl_document_node_count(umi_designer_document_declarative(document))==2U);
    assert(umi_designer_transaction_history_undo(history)==UMI_STATUS_OK); assert(umi_decl_document_node_count(umi_designer_document_declarative(document))==0U);
    assert(umi_designer_transaction_history_redo(history)==UMI_STATUS_OK); assert(umi_decl_document_node_count(umi_designer_document_declarative(document))==2U);
    umi_designer_transaction_history_destroy(history); umi_designer_document_destroy(document); return EXIT_SUCCESS;
}
