/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_declarative_node.c
 *
 * PURPOSE:
 *   Verify semantic nodes own typed properties and support update/removal.
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
    UmiDeclNode node;
    UmiDeclAttribute attribute;
    assert(umi_decl_node_init(&node,"editor","editor","root")==UMI_STATUS_OK);
    assert(umi_decl_node_set_attribute(&node,"title",UMI_DECL_VALUE_STRING,"Source")==UMI_STATUS_OK);
    assert(umi_decl_node_get_attribute(&node,"title",&attribute)==UMI_STATUS_OK);
    assert(strcmp(attribute.value.text,"Source")==0);
    assert(umi_decl_node_remove_attribute(&node,"title")==UMI_STATUS_OK);
    return EXIT_SUCCESS;
}
