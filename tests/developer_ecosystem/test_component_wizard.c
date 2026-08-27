/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_ecosystem/test_component_wizard.c
 * PURPOSE: Verify template selection and dependency-aware scaffold previews.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer/ecosystem/component_wizard.h"

int main(void)
{
    UmiEcosystemComponentWizard wizard;
    UmiEcosystemComponentTemplate component_template;
    UmiEcosystemComponentScaffoldPlan plan;
    umi_ecosystem_component_wizard_init(&wizard);
    umi_ecosystem_component_template_init(&component_template,
        "framework-panel", "Framework Panel", UMI_ECOSYSTEM_COMPONENT_PANEL);
    assert(umi_ecosystem_copy_text(component_template.package_id,
        sizeof(component_template.package_id), "org.umicom.ui") == UMI_STATUS_OK);
    assert(umi_ecosystem_copy_text(component_template.required_sdk_id,
        sizeof(component_template.required_sdk_id), "umicom-1") == UMI_STATUS_OK);
    assert(umi_ecosystem_component_wizard_add_template(&wizard,
        &component_template) == UMI_STATUS_OK);
    assert(umi_ecosystem_component_wizard_select(&wizard, "framework-panel") ==
        UMI_STATUS_OK);
    assert(umi_ecosystem_component_wizard_build_plan(&wizard, "risk_panel",
        "Risk Panel", "umicom_risk", "C:/work/risk", false, false,
        &plan) == UMI_STATUS_OK);
    assert(!plan.ready && plan.blocker_count == 2U && plan.file_count == 4U);
    assert(umi_ecosystem_component_wizard_build_plan(&wizard, "risk_panel",
        "Risk Panel", "umicom_risk", "C:/work/risk", true, true,
        &plan) == UMI_STATUS_OK);
    assert(plan.ready && plan.file_count == 4U);
    assert(!umi_ecosystem_component_identifier_valid("9invalid"));
    return 0;
}
