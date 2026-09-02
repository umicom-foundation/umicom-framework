/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_support_acceptance/test_all_tools.c
 *
 * PURPOSE:
 *   Verify the existing language/toolchain support matrix all tools behavior.
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
/*
 * Exercise available and return a clear result when the behaviour no longer matches its
 * contract.
 */
static int available(const char *program, void *user_data)
{
    (void)user_data;
    return program != NULL && program[0] != '\0';
}
/*
 * Exercise test language support all tools and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_language_support_all_tools(void)
{
    UmiLanguageProfileRegistry *profiles = NULL;
    UmiDeveloperToolchainBindingRegistry *bindings = NULL;
    UmiDeveloperLanguageSupportMatrix matrix;
    assert(umi_language_profile_registry_create(&profiles) == UMI_STATUS_OK);
    assert(umi_language_profile_register_builtins(profiles) == UMI_STATUS_OK);
    assert(umi_developer_toolchain_binding_registry_create(&bindings) == UMI_STATUS_OK);
    assert(umi_developer_toolchain_binding_register_builtins(bindings) == UMI_STATUS_OK);
    assert(umi_developer_language_support_matrix_build(
        profiles, bindings, "*", "*", available, NULL, &matrix) == UMI_STATUS_OK);
    assert(matrix.count == umi_language_profile_registry_count(profiles));
    assert(matrix.binding_found_count == matrix.count);
    assert(matrix.fully_ready_count + matrix.blocked_count == matrix.count);
    assert(matrix.blocked_count > 0U);
    umi_developer_toolchain_binding_registry_destroy(bindings);
    umi_language_profile_registry_destroy(profiles);
    return 0;
}
