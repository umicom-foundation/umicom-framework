/*-----------------------------------------------------------------------------
 * Umicom Framework - Visual Application Builder v2 types/tree tests
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: This executable example verifies stable labels and hierarchy projection. */
#include "umicom/umicom.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
int main(void)
{
    UmiDesignerBuilderSessionV2 *session=NULL; UmiDesignerComponentTreeV2 tree;
    assert(strcmp(umi_designer_binding_mode_v2_text(UMI_DESIGNER_BIND_TWO_WAY),"two-way")==0);
    assert(strcmp(umi_designer_drop_position_v2_text(UMI_DESIGNER_DROP_AFTER),"after")==0);
    assert(umi_designer_builder_session_v2_create("org.umicom.tree",&session)==UMI_STATUS_OK);
    assert(umi_designer_builder_session_v2_add_component(session,"panel","pane","root")==UMI_STATUS_OK);
    assert(umi_designer_builder_session_v2_add_component(session,"save","button","panel")==UMI_STATUS_OK);
    assert(umi_designer_builder_session_v2_select(session,"save",0)==UMI_STATUS_OK);
    assert(umi_designer_builder_session_v2_tree(session,&tree)==UMI_STATUS_OK);
    assert(tree.count==3U); assert(umi_designer_component_tree_v2_find(&tree,"save")->depth==2U);
    assert(umi_designer_component_tree_v2_find(&tree,"save")->selected!=0);
    assert(umi_designer_component_tree_v2_reparent(umi_designer_builder_session_v2_history(session),umi_designer_builder_session_v2_document(session),"root","save",0U)==UMI_STATUS_INVALID_STATE);
    umi_designer_builder_session_v2_destroy(session); return EXIT_SUCCESS;
}
