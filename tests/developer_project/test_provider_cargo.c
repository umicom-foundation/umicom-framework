/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_provider_cargo.c
 *
 * PURPOSE:
 *   Verify the built-in Cargo provider contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_project/providers/cargo.h"

int main(void)
{
    const UmiDeveloperProjectLanguageProvider *provider = umi_developer_project_provider_cargo();

    assert(provider != NULL);
    assert(umi_developer_project_language_provider_validate(provider) ==
           UMI_STATUS_OK);
    return 0;
}
