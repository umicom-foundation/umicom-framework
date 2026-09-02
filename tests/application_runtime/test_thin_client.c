/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_thin_client.c
 *
 * PURPOSE:
 *   Verify the shared thin-client helper resolves canonical application
 *   contracts and reuses the existing workspace/readiness runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/thin_client.h"

#include <string.h>

/*
 * Exercise capability available and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int capability_available(const char *capability_id, void *user_data)
{
    (void)user_data;
    return capability_id != NULL;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiApplicationThinClient client;
    UmiApplicationThinClient *created = NULL;
    UmiApplicationRuntimeHealth health;
    UmiStatus status;

    status = umi_application_thin_client_init("org.umicom.llm", &client);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK)
        return 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (client.contract.experience == NULL ||
        strcmp(client.contract.experience->application_id, "org.umicom.llm") != 0)
        return 2;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (client.workspace.session.layout == NULL)
        return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (client.readiness.feature_count !=
        client.contract.experience->feature_count)
        return 4;

    status = umi_application_thin_client_select_layout(
        &client, client.contract.experience->default_layout_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK)
        return 5;

    status = umi_application_thin_client_set_layout_locked(&client, true);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK)
        return 6;
    status = umi_application_thin_client_set_layout_locked(&client, false);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK)
        return 7;

    status = umi_application_thin_client_health(
        &client, capability_available, NULL, &health);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK)
        return 8;

    status = umi_application_thin_client_create(
        "org.umicom.llm", &created);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status != UMI_STATUS_OK || created == NULL)
        return 9;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (created->contract.experience == NULL ||
        strcmp(created->contract.experience->application_id,
               "org.umicom.llm") != 0) {
        umi_application_thin_client_destroy(created);
        return 10;
    }
    umi_application_thin_client_destroy(created);

    return 0;
}
