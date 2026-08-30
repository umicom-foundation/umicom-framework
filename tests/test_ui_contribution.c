/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_contribution.c
 *
 * PURPOSE:
 *   Verify extension-point contributions retain provider and target identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(void)
{
    UmiUiContributionModel *model = NULL;
    UmiUiContributionSnapshot contribution = {0};
    assert(umi_ui_contribution_model_create(&model) == UMI_STATUS_OK);
    (void)snprintf(contribution.contribution_id, sizeof(contribution.contribution_id), "%s", "studio.explorer.pane");
    (void)snprintf(contribution.provider_id, sizeof(contribution.provider_id), "%s", "umicom.studio");
    (void)snprintf(contribution.extension_point, sizeof(contribution.extension_point), "%s", "workbench.panes");
    (void)snprintf(contribution.target_id, sizeof(contribution.target_id), "%s", "studio.explorer");
    contribution.enabled = 1;
    assert(umi_ui_contribution_model_upsert(model, &contribution) == UMI_STATUS_OK);
    assert(umi_ui_contribution_model_count(model) == 1U);
    umi_ui_contribution_model_destroy(model);
    return EXIT_SUCCESS;
}
