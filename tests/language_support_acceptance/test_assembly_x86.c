/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_support_acceptance/test_assembly_x86.c
 *
 * PURPOSE:
 *   Verify the existing language/toolchain support matrix assembly x86 behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/developer/language_support_matrix.h"
static int available(const char *program, void *user_data){(void)user_data;return program!=NULL&&program[0]!='\0';}
int test_language_support_assembly_x86(void)
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
        profiles,bindings,"linux","x86_64",available,NULL,&matrix)==UMI_STATUS_OK);
    row=umi_developer_language_support_matrix_find(&matrix,"assembly");
    assert(row!=NULL);
    assert(strcmp(row->binding_id,"assembly.nasm")==0);
    umi_developer_toolchain_binding_registry_destroy(bindings);
    umi_language_profile_registry_destroy(profiles);
    return 0;
}
