/*-----------------------------------------------------------------------------
 * Umicom Framework - Visual Application Builder v2 property tests
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Draft validation prevents invalid text from mutating the document. */
#include "umicom/umicom.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
int main(void)
{
    UmiDesignerBuilderSessionV2 *session=NULL; UmiDesignerPropertyDraftV2 draft; UmiDeclNode node; UmiDeclAttribute attribute;
    assert(umi_designer_builder_session_v2_create("org.umicom.property",&session)==UMI_STATUS_OK);
    assert(umi_designer_builder_session_v2_add_component(session,"save","button","root")==UMI_STATUS_OK);
    assert(umi_designer_property_editor_v2_begin(umi_designer_builder_session_v2_document(session),umi_designer_builder_session_v2_components(session),"save","visible",&draft)==UMI_STATUS_OK);
    assert(umi_designer_property_editor_v2_set(&draft,"false")==UMI_STATUS_OK);
    assert(umi_designer_property_editor_v2_commit(umi_designer_builder_session_v2_history(session),umi_designer_builder_session_v2_document(session),&draft)==UMI_STATUS_OK);
    assert(umi_decl_document_find_node(umi_designer_document_declarative(umi_designer_builder_session_v2_document(session)),"save",&node)==UMI_STATUS_OK);
    assert(umi_decl_node_get_attribute(&node,"visible",&attribute)==UMI_STATUS_OK);
    assert(strcmp(attribute.value.text,"false")==0);
    umi_designer_builder_session_v2_destroy(session); return EXIT_SUCCESS;
}
