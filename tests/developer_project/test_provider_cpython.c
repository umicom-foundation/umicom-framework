/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_provider_cpython.c
 *
 * PURPOSE:
 *   Verify the built-in CPython Script provider contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_project/providers/cpython.h"

int main(void)
{
    const UmiDeveloperProjectLanguageProvider *provider = umi_developer_project_provider_cpython();

    assert(provider != NULL);
    assert(umi_developer_project_language_provider_validate(provider) ==
           UMI_STATUS_OK);
    return 0;
}
