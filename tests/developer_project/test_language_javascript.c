/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_language_javascript.c
 *
 * PURPOSE:
 *   Verify the built-in JavaScript language-pack contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_project/languages/javascript.h"

int main(void)
{
    const UmiDeveloperProjectLanguagePack *pack = umi_developer_project_language_javascript();

    assert(pack != NULL);
    assert(umi_developer_project_language_pack_validate(pack) == UMI_STATUS_OK);
    assert(pack->extension_count == 3U);
    assert(umi_developer_project_language_pack_supports_extension(
        pack, ".js"));
    return 0;
}
