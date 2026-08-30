/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_provider_zig.c
 *
 * PURPOSE:
 *   Verify the built-in Zig Build provider contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_project/providers/zig.h"

int main(void)
{
    const UmiDeveloperProjectLanguageProvider *provider = umi_developer_project_provider_zig();

    assert(provider != NULL);
    assert(umi_developer_project_language_provider_validate(provider) ==
           UMI_STATUS_OK);
    return 0;
}
