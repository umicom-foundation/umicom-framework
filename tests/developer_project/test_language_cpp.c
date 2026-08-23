/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_language_cpp.c
 *
 * PURPOSE:
 *   Verify the built-in C++ language-pack contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_project/languages/cpp.h"

int main(void)
{
    const UmiDeveloperProjectLanguagePack *pack = umi_developer_project_language_cpp();

    assert(pack != NULL);
    assert(umi_developer_project_language_pack_validate(pack) == UMI_STATUS_OK);
    assert(pack->extension_count == 5U);
    assert(umi_developer_project_language_pack_supports_extension(
        pack, ".cc"));
    return 0;
}
