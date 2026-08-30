/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_designer_v2_binding.c
 *
 * PURPOSE:
 *   Implement the test designer v2 binding behavior for
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
    UmiDesignerBuilderSessionV2 *session=NULL; UmiDesignerBindingV2 binding={0}; UmiDesignerBindingV2 found;
    assert(umi_designer_builder_session_v2_create("org.umicom.binding",&session)==UMI_STATUS_OK);
    assert(umi_designer_builder_session_v2_add_component(session,"title","label","root")==UMI_STATUS_OK);
    strcpy(binding.binding_id,"title-text"); strcpy(binding.node_id,"title"); strcpy(binding.property_name,"title"); strcpy(binding.source_expression,"state.customer_name"); binding.mode=UMI_DESIGNER_BIND_ONE_WAY; binding.enabled=1;
    assert(umi_designer_binding_editor_v2_upsert(umi_designer_builder_session_v2_bindings(session),umi_designer_builder_session_v2_document(session),umi_designer_builder_session_v2_components(session),&binding)==UMI_STATUS_OK);
    assert(umi_designer_binding_editor_v2_find(umi_designer_builder_session_v2_bindings(session),"title-text",&found)==UMI_STATUS_OK);
    assert(strcmp(found.source_expression,"state.customer_name")==0);
    assert(umi_designer_binding_editor_v2_count(umi_designer_builder_session_v2_bindings(session))==1U);
    umi_designer_builder_session_v2_destroy(session); return EXIT_SUCCESS;
}
