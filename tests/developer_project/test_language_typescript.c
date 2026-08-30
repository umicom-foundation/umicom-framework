/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_language_typescript.c
 *
 * PURPOSE:
 *   Verify the built-in TypeScript language-pack contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_project/languages/typescript.h"

int main(void)
{
    const UmiDeveloperProjectLanguagePack *pack = umi_developer_project_language_typescript();

    assert(pack != NULL);
    assert(umi_developer_project_language_pack_validate(pack) == UMI_STATUS_OK);
    assert(pack->extension_count == 2U);
    assert(umi_developer_project_language_pack_supports_extension(
        pack, ".ts"));
    return 0;
}
