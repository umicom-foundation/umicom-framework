/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_language_yaml.c
 *
 * PURPOSE:
 *   Verify the built-in YAML language-pack contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_project/languages/yaml.h"

int main(void)
{
    const UmiDeveloperProjectLanguagePack *pack = umi_developer_project_language_yaml();

    assert(pack != NULL);
    assert(umi_developer_project_language_pack_validate(pack) == UMI_STATUS_OK);
    assert(pack->extension_count == 2U);
    assert(umi_developer_project_language_pack_supports_extension(
        pack, ".yaml"));
    return 0;
}
