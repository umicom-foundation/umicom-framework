/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_designer_interaction.c
 *
 * PURPOSE:
 *   Implement the test designer interaction behavior for
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
 * Umicom Framework - Visual Application Builder v2 interaction tests
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Command identifiers replace hidden widget callback pointers. */
#include "umicom/umicom.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
int main(void)
{
    UmiDesignerBuilderSession *session=NULL; UmiDesignerInteraction interaction={0}; UmiDesignerInteraction found;
    assert(umi_designer_builder_session_create("org.umicom.interaction",&session)==UMI_STATUS_OK);
    assert(umi_designer_builder_session_add_component(session,"save","button","root")==UMI_STATUS_OK);
    strcpy(interaction.interaction_id,"save-click"); strcpy(interaction.node_id,"save"); strcpy(interaction.source_name,"clicked"); strcpy(interaction.command_id,"document.save"); interaction.kind=UMI_DESIGNER_INTERACTION_SIGNAL; interaction.enabled=1;
    assert(umi_designer_interaction_editor_upsert(umi_designer_builder_session_interactions(session),umi_designer_builder_session_document(session),&interaction)==UMI_STATUS_OK);
    assert(umi_designer_interaction_editor_at(umi_designer_builder_session_interactions(session),0U,&found)==UMI_STATUS_OK);
    assert(strcmp(found.command_id,"document.save")==0);
    umi_designer_builder_session_destroy(session); return EXIT_SUCCESS;
}
