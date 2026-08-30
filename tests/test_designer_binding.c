/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_designer_binding.c
 *
 * PURPOSE:
 *   Implement the test designer binding behavior for
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
 * Umicom Framework - Visual Application Builder v2 binding tests
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Bindings are validated against both semantic nodes and typed properties. */
#include "umicom/umicom.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
int main(void)
{
    UmiDesignerBuilderSession *session=NULL; UmiDesignerBinding binding={0}; UmiDesignerBinding found;
    assert(umi_designer_builder_session_create("org.umicom.binding",&session)==UMI_STATUS_OK);
    assert(umi_designer_builder_session_add_component(session,"title","label","root")==UMI_STATUS_OK);
    strcpy(binding.binding_id,"title-text"); strcpy(binding.node_id,"title"); strcpy(binding.property_name,"title"); strcpy(binding.source_expression,"state.customer_name"); binding.mode=UMI_DESIGNER_BIND_ONE_WAY; binding.enabled=1;
    assert(umi_designer_binding_editor_upsert(umi_designer_builder_session_bindings(session),umi_designer_builder_session_document(session),umi_designer_builder_session_components(session),&binding)==UMI_STATUS_OK);
    assert(umi_designer_binding_editor_find(umi_designer_builder_session_bindings(session),"title-text",&found)==UMI_STATUS_OK);
    assert(strcmp(found.source_expression,"state.customer_name")==0);
    assert(umi_designer_binding_editor_count(umi_designer_builder_session_bindings(session))==1U);
    umi_designer_builder_session_destroy(session); return EXIT_SUCCESS;
}
