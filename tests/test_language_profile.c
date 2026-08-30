/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_language_profile.c
 *
 * PURPOSE:
 *   Implement the test language profile behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework universal language profile regression test.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT. */
#include <assert.h>
#include <string.h>

#include "umicom/language/profile.h"

int main(void)
{
    UmiLanguageProfileRegistry *registry = NULL;
    UmiLanguageProfileRegistrySnapshot registry_snapshot;
    UmiLanguageProfileSnapshot profile;

    assert(umi_language_profile_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_language_profile_register_builtins(registry) == UMI_STATUS_OK);
    assert(umi_language_profile_registry_snapshot(registry, &registry_snapshot) ==
           UMI_STATUS_OK);
    assert(registry_snapshot.profile_count ==
           umi_language_profile_builtin_count());
    assert(registry_snapshot.profile_count >= 20U);
    assert(registry_snapshot.native_compiled_count >= 5U);
    assert(registry_snapshot.transpiled_to_c_count >= 2U);

    assert(umi_language_profile_registry_find_for_path(
               registry, "source/runtime/master_controller.c", &profile) ==
           UMI_STATUS_OK);
    assert(strcmp(profile.id, "c") == 0);
    assert(umi_language_profile_supports(
        &profile, UMI_LANGUAGE_CAPABILITY_BUILD |
                  UMI_LANGUAGE_CAPABILITY_MACHINE_CODE));
    assert(strcmp(profile.default_standard, "C23") == 0);

    assert(umi_language_profile_registry_find_for_path(
               registry, "include/engine/controller.hpp", &profile) ==
           UMI_STATUS_OK);
    assert(strcmp(profile.id, "cpp") == 0);
    assert(umi_language_profile_registry_find_for_path(
               registry, "project/CMakeLists.txt", &profile) == UMI_STATUS_OK);
    assert(strcmp(profile.id, "cmake") == 0);
    assert(umi_language_profile_registry_find_for_path(
               registry, "docs/README", &profile) == UMI_STATUS_OK);
    assert(strcmp(profile.id, "markdown") == 0);
    assert(umi_language_profile_registry_find_for_path(
               registry, "src/unknown.future", &profile) == UMI_STATUS_NOT_FOUND);

    assert(strcmp(umi_language_execution_kind_text(
                      UMI_LANGUAGE_EXECUTION_TRANSPILED_TO_C),
                  "transpiled-to-c") == 0);
    umi_language_profile_registry_destroy(registry);
    return 0;
}
