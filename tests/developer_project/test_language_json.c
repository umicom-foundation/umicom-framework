/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_language_json.c
 *
 * PURPOSE:
 *   Verify the built-in JSON language-pack contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_project/languages/json.h"

int main(void)
{
    const UmiDeveloperProjectLanguagePack *pack = umi_developer_project_language_json();

    assert(pack != NULL);
    assert(umi_developer_project_language_pack_validate(pack) == UMI_STATUS_OK);
    assert(pack->extension_count == 2U);
    assert(umi_developer_project_language_pack_supports_extension(
        pack, ".json"));
    return 0;
}
