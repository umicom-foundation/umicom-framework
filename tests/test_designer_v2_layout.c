/*-----------------------------------------------------------------------------
 * Umicom Framework - Visual Application Builder v2 layout tests
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Alignment records geometry changes as one undoable transaction. */
#include "umicom/umicom.h"
#include <assert.h>
#include <stdlib.h>
int main(void)
{
    UmiDesignerBuilderSessionV2 *session=NULL; UmiDesignerSelection *selection; UmiDesignerRect left={5,10,100,30},right={55,60,100,30},after;
    assert(umi_designer_builder_session_v2_create("org.umicom.layout",&session)==UMI_STATUS_OK);
    assert(umi_designer_builder_session_v2_add_component(session,"left","button","root")==UMI_STATUS_OK); assert(umi_designer_builder_session_v2_add_component(session,"right","button","root")==UMI_STATUS_OK);
    assert(umi_designer_surface_set_rect(umi_designer_builder_session_v2_document(session),"left",left)==UMI_STATUS_OK); assert(umi_designer_surface_set_rect(umi_designer_builder_session_v2_document(session),"right",right)==UMI_STATUS_OK);
    selection=umi_designer_builder_session_v2_selection(session); umi_designer_selection_clear(selection); assert(umi_designer_selection_set_primary(selection,"left")==UMI_STATUS_OK); assert(umi_designer_selection_add(selection,"right")==UMI_STATUS_OK);
    assert(umi_designer_layout_v2_align(umi_designer_builder_session_v2_document(session),umi_designer_builder_session_v2_transactions(session),selection,UMI_DESIGNER_ALIGN_LEFT)==UMI_STATUS_OK);
    assert(umi_designer_surface_get_rect(umi_designer_builder_session_v2_document(session),"right",&after)==UMI_STATUS_OK); assert(after.x==5);
    umi_designer_builder_session_v2_destroy(session); return EXIT_SUCCESS;
}
