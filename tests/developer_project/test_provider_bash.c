/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_provider_bash.c
 *
 * PURPOSE:
 *   Verify the built-in Bash Script provider contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_project/providers/bash.h"

int main(void)
{
    const UmiDeveloperProjectLanguageProvider *provider = umi_developer_project_provider_bash();

    assert(provider != NULL);
    assert(umi_developer_project_language_provider_validate(provider) ==
           UMI_STATUS_OK);
    return 0;
}
