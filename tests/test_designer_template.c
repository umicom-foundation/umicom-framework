/*-----------------------------------------------------------------------------
 * Umicom Framework - Visual Application Builder v2 template tests
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: A named template expands into ordinary inspectable document nodes. */
#include "umicom/umicom.h"
#include <assert.h>
#include <stdlib.h>
int main(void)
{
    UmiDesignerBuilderSession *session=NULL; UmiDesignerTemplate item; UmiDeclNode node;
    assert(umi_designer_builder_session_create("org.umicom.template",&session)==UMI_STATUS_OK);
    assert(umi_designer_template_registry_find(umi_designer_builder_session_templates(session),"form-card",&item)==UMI_STATUS_OK);
    assert(umi_designer_template_instantiate(&item,umi_designer_builder_session_transactions(session),"root","customer")==UMI_STATUS_OK);
    assert(umi_decl_document_find_node(umi_designer_document_declarative(umi_designer_builder_session_document(session)),"customer_submit",&node)==UMI_STATUS_OK);
    assert(item.node_count==3U); umi_designer_builder_session_destroy(session); return EXIT_SUCCESS;
}
