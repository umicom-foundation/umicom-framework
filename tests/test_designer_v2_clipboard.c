/*-----------------------------------------------------------------------------
 * Umicom Framework - Visual Application Builder v2 clipboard tests
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Copy includes descendants and paste remaps their stable identifiers. */
#include "umicom/umicom.h"
#include <assert.h>
#include <stdlib.h>
int main(void)
{
    UmiDesignerBuilderSessionV2 *session=NULL; UmiDeclNode node;
    assert(umi_designer_builder_session_v2_create("org.umicom.clipboard",&session)==UMI_STATUS_OK);
    assert(umi_designer_builder_session_v2_add_component(session,"card","pane","root")==UMI_STATUS_OK);
    assert(umi_designer_builder_session_v2_add_component(session,"label","label","card")==UMI_STATUS_OK);
    assert(umi_designer_builder_session_v2_select(session,"card",0)==UMI_STATUS_OK);
    assert(umi_designer_clipboard_v2_copy(umi_designer_builder_session_v2_clipboard(session),umi_designer_builder_session_v2_document(session),umi_designer_builder_session_v2_selection(session))==UMI_STATUS_OK);
    assert(umi_designer_clipboard_v2_paste(umi_designer_builder_session_v2_clipboard(session),umi_designer_builder_session_v2_document(session),umi_designer_builder_session_v2_transactions(session),"root","copy")==UMI_STATUS_OK);
    assert(umi_decl_document_find_node(umi_designer_document_declarative(umi_designer_builder_session_v2_document(session)),"copy_label",&node)==UMI_STATUS_OK);
    assert(umi_designer_transaction_history_v2_undo_count(umi_designer_builder_session_v2_transactions(session))==1U);
    umi_designer_builder_session_v2_destroy(session); return EXIT_SUCCESS;
}
