/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web_workbench/test_cloud_object.c
 * PURPOSE: Verify credential-referenced cloud profiles and object inventory.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/web/workbench/cloud_object.h"

int main(void)
{
    UmiWebWorkbenchCloudProfile profile;
    UmiWebWorkbenchCloudObjectModel *model = calloc(1U, sizeof(*model));
    UmiWebWorkbenchCloudObject object;
    const UmiWebWorkbenchCloudObject *matches[4U];
    assert(model != NULL);
    umi_web_workbench_cloud_profile_init(&profile, "aws-dev", "AWS Dev",
        UMI_WEB_WORKBENCH_CLOUD_AWS);
    assert(umi_web_workbench_copy_text(profile.secret_reference,
        sizeof(profile.secret_reference), "secret://aws/dev") == UMI_STATUS_OK);
    assert(umi_web_workbench_cloud_profile_validate(&profile) == UMI_STATUS_OK);
    umi_web_workbench_cloud_object_model_init(model, &profile);
    memset(&object, 0, sizeof(object));
    assert(umi_web_workbench_copy_text(object.bucket, sizeof(object.bucket),
        "reports") == UMI_STATUS_OK);
    assert(umi_web_workbench_copy_text(object.key, sizeof(object.key),
        "daily/pnl.json") == UMI_STATUS_OK);
    assert(umi_web_workbench_copy_text(object.content_type,
        sizeof(object.content_type), "application/json") == UMI_STATUS_OK);
    object.size_bytes = 1024U;
    assert(umi_web_workbench_cloud_object_upsert(model, &object) == UMI_STATUS_OK);
    assert(umi_web_workbench_cloud_object_query(model, "reports", "pnl",
        matches, 4U) == 1U);
    assert(umi_web_workbench_cloud_object_remove(model, "reports",
        "daily/pnl.json") == UMI_STATUS_OK);
    free(model);
    return 0;
}
