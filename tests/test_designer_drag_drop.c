/*-----------------------------------------------------------------------------
 * Umicom Framework - Visual Application Builder v2 drag/drop tests
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Pointer details stay outside Framework; completed drops mutate semantic state. */
#include "umicom/umicom.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
int main(void)
{
    UmiDesignerBuilderSession *session=NULL; UmiDesignerDragDrop drag; UmiDeclNode node;
    assert(umi_designer_builder_session_create("org.umicom.drop",&session)==UMI_STATUS_OK);
    assert(umi_designer_drag_drop_begin_palette(&drag,"button","new_button")==UMI_STATUS_OK);
    assert(umi_designer_drag_drop_target(&drag,"root",UMI_DESIGNER_DROP_INTO)==UMI_STATUS_OK);
    assert(umi_designer_drag_drop_commit(&drag,umi_designer_builder_session_history(session),umi_designer_builder_session_document(session))==UMI_STATUS_OK);
    assert(umi_decl_document_find_node(umi_designer_document_declarative(umi_designer_builder_session_document(session)),"new_button",&node)==UMI_STATUS_OK);
    assert(strcmp(node.parent_id,"root")==0); umi_designer_builder_session_destroy(session); return EXIT_SUCCESS;
}
