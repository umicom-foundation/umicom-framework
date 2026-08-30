/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_designer_v2_session.c
 *
 * PURPOSE:
 *   Implement the test designer v2 session behavior for
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
/* BEGINNER NOTE: Preview and generated source always derive from the same document revision. */
#include "umicom/umicom.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
int main(void)
{
    UmiDesignerBuilderSessionV2 *session=NULL; UmiDesignerBuilderSessionSnapshotV2 snapshot; const UmiDesignerGeneratedFileV2 *file;
    assert(umi_designer_builder_session_v2_create("org.umicom.session",&session)==UMI_STATUS_OK);
    assert(umi_designer_builder_session_v2_add_component(session,"hello","label","root")==UMI_STATUS_OK);
    assert(umi_designer_builder_session_v2_preview(session)==UMI_STATUS_OK); assert(strstr(umi_designer_live_preview_v2_text(umi_designer_builder_session_v2_live_preview(session)),"hello:label")!=NULL);
    assert(umi_designer_builder_session_v2_generate(session)==UMI_STATUS_OK); file=umi_designer_source_generation_v2_find(umi_designer_builder_session_v2_generated_source(session),"application.umiapp"); assert(file!=NULL); assert(strstr(file->content,"hello")!=NULL);
    assert(umi_designer_builder_session_v2_snapshot(session,&snapshot)==UMI_STATUS_OK); assert(snapshot.preview_health==UMI_DESIGNER_PREVIEW_CURRENT); assert(snapshot.generated_revision==snapshot.document.revision);
    umi_designer_builder_session_v2_destroy(session); return EXIT_SUCCESS;
}
