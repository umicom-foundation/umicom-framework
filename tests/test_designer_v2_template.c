/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_designer_v2_template.c
 *
 * PURPOSE:
 *   Implement the test designer v2 template behavior for
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
 * Umicom Framework - Visual Application Builder v2 template tests
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: A named template expands into ordinary inspectable document nodes. */
#include "umicom/umicom.h"
#include <assert.h>
#include <stdlib.h>
int main(void)
{
    UmiDesignerBuilderSessionV2 *session=NULL; UmiDesignerTemplateV2 item; UmiDeclNode node;
    assert(umi_designer_builder_session_v2_create("org.umicom.template",&session)==UMI_STATUS_OK);
    assert(umi_designer_template_registry_v2_find(umi_designer_builder_session_v2_templates(session),"form-card",&item)==UMI_STATUS_OK);
    assert(umi_designer_template_v2_instantiate(&item,umi_designer_builder_session_v2_transactions(session),"root","customer")==UMI_STATUS_OK);
    assert(umi_decl_document_find_node(umi_designer_document_declarative(umi_designer_builder_session_v2_document(session)),"customer_submit",&node)==UMI_STATUS_OK);
    assert(item.node_count==3U); umi_designer_builder_session_v2_destroy(session); return EXIT_SUCCESS;
}
