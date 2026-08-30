/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_support_acceptance/test_document.c
 *
 * PURPOSE:
 *   Verify the existing language/toolchain support matrix document behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/developer/language_support_matrix.h"
static int available(const char *program, void *user_data){(void)user_data;return program!=NULL&&program[0]!='\0';}
int test_language_support_document(void)
{
    UmiLanguageProfileRegistry *profiles=NULL;
    UmiDeveloperToolchainBindingRegistry *bindings=NULL;
    UmiDeveloperLanguageSupportMatrix matrix;
    const UmiDeveloperLanguageSupportSnapshot *row;
    assert(umi_language_profile_registry_create(&profiles)==UMI_STATUS_OK);
    assert(umi_language_profile_register_builtins(profiles)==UMI_STATUS_OK);
    assert(umi_developer_toolchain_binding_registry_create(&bindings)==UMI_STATUS_OK);
    assert(umi_developer_toolchain_binding_register_builtins(bindings)==UMI_STATUS_OK);
    assert(umi_developer_language_support_matrix_build(
        profiles,bindings,"*","*",available,NULL,&matrix)==UMI_STATUS_OK);
    row=umi_developer_language_support_matrix_find(&matrix,"markdown");
    assert(row!=NULL&&row->binding_found);
    assert(row->supported_operation_count>=2U);
    umi_developer_toolchain_binding_registry_destroy(bindings);
    umi_language_profile_registry_destroy(profiles);
    return 0;
}
