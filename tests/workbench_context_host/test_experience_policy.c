/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_experience_policy.c
 *
 * PURPOSE:
 *   Verify shared experience policies resolve development, banking, AI and
 *   media panels without application-owned context-link code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>

#include "umicom/application/experience_catalogue.h"
#include "umicom/workbench_context_host/experience_policy.h"

/*
 * Exercise resolve and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiWorkbenchContextHostExperiencePanelPolicy resolve(
    const char *application_id,
    const char *panel_id)
{
    UmiWorkbenchContextHostExperiencePanelPolicy policy;
    const UmiApplicationExperienceDefinition *experience =
        umi_application_experience_catalogue_find(application_id);
    const UmiExperiencePanelDefinition *panel;

    assert(experience != NULL);
    panel = umi_application_experience_panel_find(experience, panel_id);
    assert(panel != NULL);
    assert(umi_workbench_context_host_experience_panel_policy_resolve(
        experience, panel, &policy) == UMI_STATUS_OK);
    return policy;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchContextHostExperiencePanelPolicy policy;

    policy = resolve("org.umicom.studio", "editor");
    assert(policy.role == UMI_WORKBENCH_CONTEXT_HOST_PANEL_EDITOR);
    assert(umi_workbench_context_host_kind_allowed(
        policy.accepted_kinds_mask, UMI_CONTEXT_KIND_SOURCE_LOCATION));
    assert(umi_workbench_context_host_kind_allowed(
        policy.accepted_kinds_mask, UMI_CONTEXT_KIND_PROJECT));

    policy = resolve("org.umicom.bank", "accounts");
    assert(policy.role == UMI_WORKBENCH_CONTEXT_HOST_PANEL_EXPLORER);
    assert(umi_workbench_context_host_kind_allowed(
        policy.accepted_kinds_mask, UMI_CONTEXT_KIND_ACCOUNT));

    policy = resolve("org.umicom.bank", "payments");
    assert(policy.role == UMI_WORKBENCH_CONTEXT_HOST_PANEL_PAYMENT);
    assert(policy.mode == UMI_WORKBENCH_CONTEXT_LINK_MODE_FOLLOW);
    assert(policy.published_kinds_mask == 0U);

    policy = resolve("org.umicom.rag", "source-viewer");
    assert(policy.role == UMI_WORKBENCH_CONTEXT_HOST_PANEL_DOCUMENT);
    assert(umi_workbench_context_host_kind_allowed(
        policy.accepted_kinds_mask, UMI_CONTEXT_KIND_WORKSPACE));

    policy = resolve("org.umicom.music-studio", "timeline");
    assert(policy.role == UMI_WORKBENCH_CONTEXT_HOST_PANEL_MEDIA_TIMELINE);
    assert(umi_workbench_context_host_kind_allowed(
        policy.accepted_kinds_mask, UMI_CONTEXT_KIND_MEDIA));

    return 0;
}
