/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_profile_acceptance/test_definition_catalogue.c
 *
 * PURPOSE:
 *   Verify language-intelligence definitions cover the richer profile catalogue while retaining the legacy assembly identifier.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>

#include "umicom/language/builtins.h"

int test_language_profile_acceptance_definition_catalogue(void)
{
    UmiLanguageDefinitionRegistry *registry = NULL;
    UmiLanguageDefinitionSnapshot definition;

    assert(umi_language_definition_registry_create(
        &registry) == UMI_STATUS_OK);
    assert(umi_language_register_builtin_definitions(
        registry) == UMI_STATUS_OK);

    assert(umi_language_builtin_definition_count() >= 23U);
    assert(umi_language_definition_registry_find(
        registry,
        "asm",
        &definition) == UMI_STATUS_OK);
    assert(umi_language_definition_registry_find(
        registry,
        "assembly",
        &definition) == UMI_STATUS_OK);
    assert(umi_language_definition_registry_find(
        registry,
        "java",
        &definition) == UMI_STATUS_OK);
    assert(umi_language_definition_registry_find(
        registry,
        "rust",
        &definition) == UMI_STATUS_OK);
    assert(umi_language_definition_registry_find(
        registry,
        "umicc",
        &definition) == UMI_STATUS_OK);

    umi_language_definition_registry_destroy(registry);
    return 0;
}
