/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_selection_provider/test_studio_profile.c
 *
 * PURPOSE:
 *   Verify standard Studio model providers use panel-current routing and verify
 *   the reusable provider runtime pipeline can be built around a Context Host.
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

#include "umicom/workbench_context_link/service.h"
#include "umicom/workbench_context_source/studio_profile.h"
#include "umicom/workbench_selection_provider/service.h"
#include "umicom/workbench_selection_provider/studio_profile.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchSelectionProviderStudioProfile provider_profile =
        umi_workbench_selection_provider_studio_profile_default();
    UmiWorkbenchContextSourceStudioProfile source_profile =
        umi_workbench_context_source_studio_profile_default();
    UmiWorkbenchContextLinkService links;
    UmiWorkbenchContextHostConfig host_config;
    UmiWorkbenchContextHost *host = NULL;
    UmiWorkbenchSelectionProviderPipeline *pipeline = NULL;

    assert(strcmp(provider_profile.application_id, "org.umicom.studio") == 0);
    assert(strcmp(provider_profile.project_panel_id,
                  "studio.project-explorer") == 0);
    assert(strcmp(provider_profile.problems_panel_id, "studio.problems") == 0);
    assert(strcmp(provider_profile.source_control_panel_id,
                  "studio.source-control") == 0);
    assert(strcmp(provider_profile.test_panel_id, "studio.test-explorer") == 0);

    /*
     * The pipeline must remain usable with a caller-owned Context Host.  The
     * test intentionally creates the link service and host separately so the
     * ownership boundary is exercised rather than hidden by a fixture.
     */
    umi_workbench_context_link_service_init(&links);
    host_config = umi_workbench_context_host_config_default();
    host_config.host_id = "test.studio.host";
    host_config.application_id = "org.umicom.studio";
    host_config.observer_panel_id = "studio.workbench.observer";

    assert(umi_workbench_context_host_create(
               &host_config, &links, &host) == UMI_STATUS_OK);
    assert(umi_workbench_selection_provider_pipeline_create(
               host, &pipeline) == UMI_STATUS_OK);

    assert(umi_workbench_selection_provider_pipeline_event_service(
               pipeline) != NULL);
    assert(umi_workbench_selection_provider_pipeline_source_service(
               pipeline) != NULL);
    assert(umi_workbench_selection_provider_pipeline_selection_service(
               pipeline) != NULL);
    assert(umi_workbench_selection_provider_pipeline_provider_service(
               pipeline) != NULL);

    /*
     * Source and provider profiles are deliberately registered by the product
     * composition rather than hard-wired into the generic pipeline.
     */
    assert(umi_workbench_context_source_register_studio_profile(
               umi_workbench_selection_provider_pipeline_source_service(
                   pipeline),
               &source_profile) == UMI_STATUS_OK);
    assert(umi_workbench_selection_provider_register_studio_profile(
               umi_workbench_selection_provider_pipeline_provider_service(
                   pipeline),
               &provider_profile) == UMI_STATUS_OK);

    umi_workbench_selection_provider_pipeline_set_suspended(pipeline, true);
    umi_workbench_selection_provider_pipeline_set_suspended(pipeline, false);

    umi_workbench_selection_provider_pipeline_destroy(pipeline);
    umi_workbench_context_host_destroy(host);
    umi_workbench_context_link_service_destroy(&links);

    return 0;
}
