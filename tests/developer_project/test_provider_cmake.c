/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_provider_cmake.c
 *
 * PURPOSE:
 *   Verify the built-in CMake/Ninja provider contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_project/providers/cmake.h"

int main(void)
{
    const UmiDeveloperProjectLanguageProvider *provider = umi_developer_project_provider_cmake();

    assert(provider != NULL);
    assert(umi_developer_project_language_provider_validate(provider) ==
           UMI_STATUS_OK);
    return 0;
}
