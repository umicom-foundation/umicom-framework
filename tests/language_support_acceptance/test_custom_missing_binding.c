/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_support_acceptance/test_custom_missing_binding.c
 *
 * PURPOSE:
 *   Verify the existing language/toolchain support matrix custom missing binding behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/developer/language_support_matrix.h"
static int available(const char *program, void *user_data){(void)user_data;return program!=NULL&&program[0]!='\0';}
int test_language_support_custom_missing_binding(void)
{
    UmiLanguageProfileRegistry *profiles=NULL;
    UmiDeveloperToolchainBindingRegistry *bindings=NULL;
    UmiDeveloperLanguageSupportMatrix matrix;
    UmiLanguageProfileSnapshot profile;
    const UmiDeveloperLanguageSupportSnapshot *row;
    assert(umi_language_profile_registry_create(&profiles)==UMI_STATUS_OK);
    assert(umi_language_profile_register_builtins(profiles)==UMI_STATUS_OK);
    assert(umi_developer_toolchain_binding_registry_create(&bindings)==UMI_STATUS_OK);
    assert(umi_developer_toolchain_binding_register_builtins(bindings)==UMI_STATUS_OK);
    umi_language_profile_init(&profile);
    strcpy(profile.id,"custom-language");
    strcpy(profile.name,"Custom Language");
    profile.enabled=true;
    profile.capabilities=UMI_LANGUAGE_CAPABILITY_RUN;
    assert(umi_language_profile_registry_upsert(profiles,&profile)==UMI_STATUS_OK);
    assert(umi_developer_language_support_matrix_build(
        profiles,bindings,"*","*",available,NULL,&matrix)==UMI_STATUS_OK);
    row=umi_developer_language_support_matrix_find(&matrix,"custom-language");
    assert(row!=NULL&&!row->binding_found);
    umi_developer_toolchain_binding_registry_destroy(bindings);
    umi_language_profile_registry_destroy(profiles);
    return 0;
}
