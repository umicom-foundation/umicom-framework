/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_designer_v2_drag_drop.c
 *
 * PURPOSE:
 *   Implement the test designer v2 drag drop behavior for
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
 * Umicom Framework - Visual Application Builder v2 drag/drop tests
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Pointer details stay outside Framework; completed drops mutate semantic state. */
#include "umicom/umicom.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
int main(void)
{
    UmiDesignerBuilderSessionV2 *session=NULL; UmiDesignerDragDropV2 drag; UmiDeclNode node;
    assert(umi_designer_builder_session_v2_create("org.umicom.drop",&session)==UMI_STATUS_OK);
    assert(umi_designer_drag_drop_v2_begin_palette(&drag,"button","new_button")==UMI_STATUS_OK);
    assert(umi_designer_drag_drop_v2_target(&drag,"root",UMI_DESIGNER_DROP_INTO)==UMI_STATUS_OK);
    assert(umi_designer_drag_drop_v2_commit(&drag,umi_designer_builder_session_v2_history(session),umi_designer_builder_session_v2_document(session))==UMI_STATUS_OK);
    assert(umi_decl_document_find_node(umi_designer_document_declarative(umi_designer_builder_session_v2_document(session)),"new_button",&node)==UMI_STATUS_OK);
    assert(strcmp(node.parent_id,"root")==0); umi_designer_builder_session_v2_destroy(session); return EXIT_SUCCESS;
}
