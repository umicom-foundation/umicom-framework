/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_language_c23.c
 *
 * PURPOSE:
 *   Verify the built-in C23 language-pack contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_project/languages/c23.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiDeveloperProjectLanguagePack *pack = umi_developer_project_language_c23();

    assert(pack != NULL);
    assert(umi_developer_project_language_pack_validate(pack) == UMI_STATUS_OK);
    assert(pack->extension_count == 2U);
    assert(umi_developer_project_language_pack_supports_extension(
        pack, ".c"));
    return 0;
}
