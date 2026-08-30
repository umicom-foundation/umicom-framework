/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_designer_clipboard.c
 *
 * PURPOSE:
 *   Implement the test designer clipboard behavior for
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
 * Umicom Framework - Visual Application Builder v2 clipboard tests
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Copy includes descendants and paste remaps their stable identifiers. */
#include "umicom/umicom.h"
#include <assert.h>
#include <stdlib.h>
int main(void)
{
    UmiDesignerBuilderSession *session=NULL; UmiDeclNode node;
    assert(umi_designer_builder_session_create("org.umicom.clipboard",&session)==UMI_STATUS_OK);
    assert(umi_designer_builder_session_add_component(session,"card","pane","root")==UMI_STATUS_OK);
    assert(umi_designer_builder_session_add_component(session,"label","label","card")==UMI_STATUS_OK);
    assert(umi_designer_builder_session_select(session,"card",0)==UMI_STATUS_OK);
    assert(umi_designer_clipboard_copy(umi_designer_builder_session_clipboard(session),umi_designer_builder_session_document(session),umi_designer_builder_session_selection(session))==UMI_STATUS_OK);
    assert(umi_designer_clipboard_paste(umi_designer_builder_session_clipboard(session),umi_designer_builder_session_document(session),umi_designer_builder_session_transactions(session),"root","copy")==UMI_STATUS_OK);
    assert(umi_decl_document_find_node(umi_designer_document_declarative(umi_designer_builder_session_document(session)),"copy_label",&node)==UMI_STATUS_OK);
    assert(umi_designer_transaction_history_undo_count(umi_designer_builder_session_transactions(session))==1U);
    umi_designer_builder_session_destroy(session); return EXIT_SUCCESS;
}
