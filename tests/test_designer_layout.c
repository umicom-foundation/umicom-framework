/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_designer_layout.c
 *
 * PURPOSE:
 *   Implement the test designer layout behavior for
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
 * Umicom Framework - Visual Application Builder v2 layout tests
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Alignment records geometry changes as one undoable transaction. */
#include "umicom/umicom.h"
#include <assert.h>
#include <stdlib.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDesignerBuilderSession *session=NULL; UmiDesignerSelection *selection; UmiDesignerRect left={5,10,100,30},right={55,60,100,30},after;
    assert(umi_designer_builder_session_create("org.umicom.layout",&session)==UMI_STATUS_OK);
    assert(umi_designer_builder_session_add_component(session,"left","button","root")==UMI_STATUS_OK); assert(umi_designer_builder_session_add_component(session,"right","button","root")==UMI_STATUS_OK);
    assert(umi_designer_surface_set_rect(umi_designer_builder_session_document(session),"left",left)==UMI_STATUS_OK); assert(umi_designer_surface_set_rect(umi_designer_builder_session_document(session),"right",right)==UMI_STATUS_OK);
    selection=umi_designer_builder_session_selection(session); umi_designer_selection_clear(selection); assert(umi_designer_selection_set_primary(selection,"left")==UMI_STATUS_OK); assert(umi_designer_selection_add(selection,"right")==UMI_STATUS_OK);
    assert(umi_designer_layout_align(umi_designer_builder_session_document(session),umi_designer_builder_session_transactions(session),selection,UMI_DESIGNER_ALIGN_LEFT)==UMI_STATUS_OK);
    assert(umi_designer_surface_get_rect(umi_designer_builder_session_document(session),"right",&after)==UMI_STATUS_OK); assert(after.x==5);
    umi_designer_builder_session_destroy(session); return EXIT_SUCCESS;
}
