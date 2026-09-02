/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_designer_session.c
 *
 * PURPOSE:
 *   Implement the test designer session behavior for
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
 * Umicom Framework - Visual Application Builder v2 session tests
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Preview and generated source always derive from the same document revision. */
#include "umicom/umicom.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDesignerBuilderSession *session=NULL; UmiDesignerBuilderSessionSnapshot snapshot; const UmiDesignerGeneratedFile *file;
    assert(umi_designer_builder_session_create("org.umicom.session",&session)==UMI_STATUS_OK);
    assert(umi_designer_builder_session_add_component(session,"hello","label","root")==UMI_STATUS_OK);
    assert(umi_designer_builder_session_preview(session)==UMI_STATUS_OK); assert(strstr(umi_designer_live_preview_text(umi_designer_builder_session_live_preview(session)),"hello:label")!=NULL);
    assert(umi_designer_builder_session_generate(session)==UMI_STATUS_OK); file=umi_designer_source_generation_find(umi_designer_builder_session_generated_source(session),"application.umiapp"); assert(file!=NULL); assert(strstr(file->content,"hello")!=NULL);
    assert(umi_designer_builder_session_snapshot(session,&snapshot)==UMI_STATUS_OK); assert(snapshot.preview_health==UMI_DESIGNER_PREVIEW_CURRENT); assert(snapshot.generated_revision==snapshot.document.revision);
    umi_designer_builder_session_destroy(session); return EXIT_SUCCESS;
}
