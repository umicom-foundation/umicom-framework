/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_declarative_templates.c
 *
 * PURPOSE:
 *   Verify named starter templates can be stored and retrieved with owned lifetime.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This test exercises the public contract rather than relying on private state,
 * making the behaviour useful as an executable example for new developers.
 */
#include "umicom/umicom.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    UmiDeclTemplateStore *store=NULL;const char *source=NULL;
    assert(umi_decl_template_store_create(&store)==UMI_STATUS_OK);
    assert(umi_decl_template_store_put(store,"starter.editor","application org.umicom.editor\ncomponent root window -\n")==UMI_STATUS_OK);
    assert(umi_decl_template_store_get(store,"starter.editor",&source)==UMI_STATUS_OK);
    assert(strstr(source,"component root")!=NULL);
    umi_decl_template_store_destroy(store);
    return EXIT_SUCCESS;
}
