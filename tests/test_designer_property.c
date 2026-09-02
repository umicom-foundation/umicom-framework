/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_designer_property.c
 *
 * PURPOSE:
 *   Implement the test designer property behavior for
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
 * Umicom Framework - Visual Application Builder v2 property tests
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Draft validation prevents invalid text from mutating the document. */
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
    UmiDesignerBuilderSession *session=NULL; UmiDesignerPropertyDraft draft; UmiDeclNode node; UmiDeclAttribute attribute;
    assert(umi_designer_builder_session_create("org.umicom.property",&session)==UMI_STATUS_OK);
    assert(umi_designer_builder_session_add_component(session,"save","button","root")==UMI_STATUS_OK);
    assert(umi_designer_property_editor_begin(umi_designer_builder_session_document(session),umi_designer_builder_session_components(session),"save","visible",&draft)==UMI_STATUS_OK);
    assert(umi_designer_property_editor_set(&draft,"false")==UMI_STATUS_OK);
    assert(umi_designer_property_editor_commit(umi_designer_builder_session_history(session),umi_designer_builder_session_document(session),&draft)==UMI_STATUS_OK);
    assert(umi_decl_document_find_node(umi_designer_document_declarative(umi_designer_builder_session_document(session)),"save",&node)==UMI_STATUS_OK);
    assert(umi_decl_node_get_attribute(&node,"visible",&attribute)==UMI_STATUS_OK);
    assert(strcmp(attribute.value.text,"false")==0);
    umi_designer_builder_session_destroy(session); return EXIT_SUCCESS;
}
