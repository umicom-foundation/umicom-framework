/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_language_sql.c
 *
 * PURPOSE:
 *   Verify the built-in SQL language-pack contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_project/languages/sql.h"

int main(void)
{
    const UmiDeveloperProjectLanguagePack *pack = umi_developer_project_language_sql();

    assert(pack != NULL);
    assert(umi_developer_project_language_pack_validate(pack) == UMI_STATUS_OK);
    assert(pack->extension_count == 1U);
    assert(umi_developer_project_language_pack_supports_extension(
        pack, ".sql"));
    return 0;
}
