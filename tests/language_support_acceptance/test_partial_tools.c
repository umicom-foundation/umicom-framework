/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_support_acceptance/test_partial_tools.c
 *
 * PURPOSE:
 *   Verify the existing language/toolchain support matrix partial tools behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/developer/language_support_matrix.h"
static int only_clang(const char *program, void *user_data)
{
    (void)user_data;
    return strcmp(program,"clang")==0 || strcmp(program,"native")==0;
}
int test_language_support_partial_tools(void)
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
        profiles,bindings,"linux","x86_64",only_clang,NULL,&matrix)==UMI_STATUS_OK);
    row=umi_developer_language_support_matrix_find(&matrix,"c");
    assert(row!=NULL&&row->binding_found);
    assert(row->missing_operation_count>0U);
    assert(row->readiness_percent<100U);
    umi_developer_toolchain_binding_registry_destroy(bindings);
    umi_language_profile_registry_destroy(profiles);
    return 0;
}
