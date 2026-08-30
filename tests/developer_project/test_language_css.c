/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_language_css.c
 *
 * PURPOSE:
 *   Verify the built-in CSS language-pack contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_project/languages/css.h"

int main(void)
{
    const UmiDeveloperProjectLanguagePack *pack = umi_developer_project_language_css();

    assert(pack != NULL);
    assert(umi_developer_project_language_pack_validate(pack) == UMI_STATUS_OK);
    assert(pack->extension_count == 3U);
    assert(umi_developer_project_language_pack_supports_extension(
        pack, ".css"));
    return 0;
}
