/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/developer_project/test_template_umicom_extension.c
 *
 * PURPOSE:
 *   Verify the community extension starter is complete and accepted by the
 *   common project-template validator.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/developer_project/templates/umicom_extension.h"

/* Check identity and essential files so Studio never advertises an extension
 * starter that cannot declare, build, or test its module entry point. */
int main(void)
{
    const UmiDeveloperProjectTemplate *project_template =
        umi_developer_project_template_umicom_extension();

    assert(project_template != NULL);
    assert(strcmp(project_template->template_id,
                  "developer.template.umicom-extension") == 0);
    assert(project_template->kind == UMI_DEVELOPER_PROJECT_EXTENSION);
    assert(project_template->file_count == 8U);
    assert(project_template->supports_tests);
    assert(project_template->supports_install);
    assert(project_template->supports_package);
    assert(umi_developer_project_template_validate(project_template) ==
           UMI_STATUS_OK);
    return 0;
}
