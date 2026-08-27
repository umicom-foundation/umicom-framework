/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_thin_client.c
 *
 * PURPOSE:
 *   Verify the shared thin-client helper resolves canonical application
 *   contracts and reuses the existing workspace/readiness runtime.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/thin_client.h"

#include <string.h>

static int capability_available(const char *capability_id, void *user_data)
{
    (void)user_data;
    return capability_id != NULL;
}

int main(void)
{
    UmiApplicationThinClient client;
    UmiApplicationThinClient *created = NULL;
    UmiApplicationRuntimeHealth health;
    UmiStatus status;

    status = umi_application_thin_client_init("org.umicom.llm", &client);
    if (status != UMI_STATUS_OK)
        return 1;
    if (client.contract.experience == NULL ||
        strcmp(client.contract.experience->application_id, "org.umicom.llm") != 0)
        return 2;
    if (client.workspace.session.layout == NULL)
        return 3;
    if (client.readiness.feature_count !=
        client.contract.experience->feature_count)
        return 4;

    status = umi_application_thin_client_select_layout(
        &client, client.contract.experience->default_layout_id);
    if (status != UMI_STATUS_OK)
        return 5;

    status = umi_application_thin_client_set_layout_locked(&client, true);
    if (status != UMI_STATUS_OK)
        return 6;
    status = umi_application_thin_client_set_layout_locked(&client, false);
    if (status != UMI_STATUS_OK)
        return 7;

    status = umi_application_thin_client_health(
        &client, capability_available, NULL, &health);
    if (status != UMI_STATUS_OK)
        return 8;

    status = umi_application_thin_client_create(
        "org.umicom.llm", &created);
    if (status != UMI_STATUS_OK || created == NULL)
        return 9;
    if (created->contract.experience == NULL ||
        strcmp(created->contract.experience->application_id,
               "org.umicom.llm") != 0) {
        umi_application_thin_client_destroy(created);
        return 10;
    }
    umi_application_thin_client_destroy(created);

    return 0;
}
