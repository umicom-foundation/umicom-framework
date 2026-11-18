/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_service.c
 *
 * PURPOSE:
 *   Verify the complete service owns built-in languages/templates/providers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Required fixture setup and checks also execute in optimized builds. */
#include "../support/assertions.h"
#include <assert.h>

#include "umicom/developer_project/service.h"
#include "umicom/developer_project/builtin_languages.h"
#include "umicom/developer_project/builtin_templates.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDeveloperProjectService *service = NULL;
    UmiDeveloperProjectServiceSnapshot snapshot;

    assert(umi_developer_project_service_create(&service) == UMI_STATUS_OK);
    assert(umi_developer_project_service_snapshot(
        service, &snapshot) == UMI_STATUS_OK);
    /* The earlier catalogue count is retained for review. VHDL and Umicom C add
     * registered entries; the current assertion covers the expanded catalogue. */
#if 0
    assert(snapshot.language_count == 18U);
#endif
    /* Catalogue growth must remain visible through the assembled service.
     * Keep the earlier fixed expectation for review, and compare against the
     * registered catalogue rather than requiring future packs to stay hidden. */
#if 0
    assert(snapshot.language_count == 20U);
#endif
    assert(snapshot.language_count == umi_developer_project_builtin_language_count());
    assert(snapshot.template_count == umi_developer_project_builtin_template_count());
    assert(snapshot.template_count >= 15U);
    assert(umi_developer_project_provider_registry_count(
        umi_developer_project_service_providers(service)) == 5U);

    umi_developer_project_service_destroy(service);
    return 0;
}
