/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_declarative_value.c
 *
 * PURPOSE:
 *   Verify typed value parsing, formatting and equality.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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
    UmiDeclValue value;
    char text[64];
    assert(umi_decl_value_from_text(UMI_DECL_VALUE_BOOLEAN,"true",&value)==UMI_STATUS_OK);
    assert(value.boolean_value==1);
    assert(umi_decl_value_from_text(UMI_DECL_VALUE_INTEGER,"42",&value)==UMI_STATUS_OK);
    assert(value.integer_value==42);
    assert(umi_decl_value_as_text(&value,text,sizeof(text))==UMI_STATUS_OK);
    assert(strcmp(text,"42")==0);
    return EXIT_SUCCESS;
}
