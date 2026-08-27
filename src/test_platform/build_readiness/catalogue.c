/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/build_readiness/catalogue.c
 * PURPOSE: Compile declarative product and executable-readiness records.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/build_readiness/catalogue.h"

#include <string.h>

#define UMI_TEST_ARTIFACT_RECORD(product, target, test, label_text, preset_name, is_required) \
    { (uint32_t)sizeof(UmiTestPlatformBuildArtifact), UMI_TEST_PLATFORM_BUILD_READINESS_API_VERSION, \
      product, target, test, label_text, preset_name, is_required }

static const UmiTestPlatformBuildArtifact ARTIFACTS[] = {
#include "catalogue/artifacts/umicom_ui_workstation_accessibility_profile_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_application_surface_set_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_asset_browser_model_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_auto_hide_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_canvas_surface_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_chart_surface_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_command_bar_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_data_grid_surface_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_density_profile_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_dock_preview_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_dock_target_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_dock_zone_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_drag_session_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_floating_window_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_focus_mode_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_inspector_model_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_kitchen_designer_layouts_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_layout_catalogue_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_layout_edit_session_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_layout_geometry_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_layout_history_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_layout_lock_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_layout_recovery_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_layout_snapshot_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_layout_template_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_layout_variant_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_maximize_mode_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_media_layouts_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_media_viewport_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_monitor_topology_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_node_graph_surface_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_os_layouts_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_palette_model_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_panel_chrome_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_panel_state_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_perspective_catalogue_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_perspective_template_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_resize_policy_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_snap_policy_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_split_region_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_status_strip_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_studio_layouts_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_surface_catalogue_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_surface_descriptor_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_tab_stack_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_theme_role_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_timeline_model_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_toolbar_model_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_trader_layouts_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_transport_model_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_types_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_waveform_surface_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_workspace_strip_test.inc"
#include "catalogue/artifacts/umicom_ui_workstation_workstation_test.inc"
#include "catalogue/artifacts/umicom_studio_experience_centre_test.inc"
#include "catalogue/artifacts/umicom_studio_experience_projection_test.inc"
#include "catalogue/artifacts/umicom_studio_application_runtime_centre_test.inc"
#include "catalogue/artifacts/umicom_studio_application_panel_centre_test.inc"
#include "catalogue/artifacts/umicom_studio_application_feature_centre_test.inc"
#include "catalogue/artifacts/umicom_studio_editor_intelligence_workbench_contribution_test.inc"
#include "catalogue/artifacts/umicom_studio_vcs_workbench_contribution_test.inc"
#include "catalogue/artifacts/umicom_studio_data_workbench_contribution_test.inc"
#include "catalogue/artifacts/umicom_studio_web_api_workbench_contribution_test.inc"
};

#undef UMI_TEST_ARTIFACT_RECORD

#define UMI_TEST_PRODUCT_PROFILE(product, name, preset_name, regex, default_on, all_modules) \
    { (uint32_t)sizeof(UmiTestPlatformProductValidationProfile), \
      UMI_TEST_PLATFORM_BUILD_READINESS_API_VERSION, product, name, preset_name, regex, \
      default_on, all_modules }

static const UmiTestPlatformProductValidationProfile PROFILES[] = {
#include "catalogue/profiles/accountant.inc"
#include "catalogue/profiles/bank.inc"
#include "catalogue/profiles/cad.inc"
#include "catalogue/profiles/creator.inc"
#include "catalogue/profiles/database_studio.inc"
#include "catalogue/profiles/desktop.inc"
#include "catalogue/profiles/education.inc"
#include "catalogue/profiles/exchange.inc"
#include "catalogue/profiles/games.inc"
#include "catalogue/profiles/integration_studio.inc"
#include "catalogue/profiles/kitchen.inc"
#include "catalogue/profiles/llm.inc"
#include "catalogue/profiles/marketplace.inc"
#include "catalogue/profiles/media.inc"
#include "catalogue/profiles/mobile_studio.inc"
#include "catalogue/profiles/music.inc"
#include "catalogue/profiles/operations.inc"
#include "catalogue/profiles/os.inc"
#include "catalogue/profiles/rag.inc"
#include "catalogue/profiles/security_centre.inc"
#include "catalogue/profiles/studio.inc"
#include "catalogue/profiles/tms.inc"
#include "catalogue/profiles/trader.inc"
#include "catalogue/profiles/web_studio.inc"
};

#undef UMI_TEST_PRODUCT_PROFILE

size_t umi_test_platform_build_artifact_catalogue_count(void)
{
    return sizeof(ARTIFACTS) / sizeof(ARTIFACTS[0]);
}

const UmiTestPlatformBuildArtifact *
umi_test_platform_build_artifact_catalogue_at(size_t index)
{
    return index < umi_test_platform_build_artifact_catalogue_count()
        ? &ARTIFACTS[index] : NULL;
}

const UmiTestPlatformBuildArtifact *
umi_test_platform_build_artifact_catalogue_find_target(const char *target_name)
{
    size_t index;
    if (target_name == NULL) return NULL;
    for (index = 0U; index < umi_test_platform_build_artifact_catalogue_count();
         ++index) {
        if (strcmp(ARTIFACTS[index].target_name, target_name) == 0)
            return &ARTIFACTS[index];
    }
    return NULL;
}

const UmiTestPlatformBuildArtifact *
umi_test_platform_build_artifact_catalogue_find_test(const char *test_name)
{
    size_t index;
    if (test_name == NULL) return NULL;
    for (index = 0U; index < umi_test_platform_build_artifact_catalogue_count();
         ++index) {
        if (strcmp(ARTIFACTS[index].test_name, test_name) == 0)
            return &ARTIFACTS[index];
    }
    return NULL;
}

size_t umi_test_platform_product_validation_profile_count(void)
{
    return sizeof(PROFILES) / sizeof(PROFILES[0]);
}

const UmiTestPlatformProductValidationProfile *
umi_test_platform_product_validation_profile_at(size_t index)
{
    return index < umi_test_platform_product_validation_profile_count()
        ? &PROFILES[index] : NULL;
}

const UmiTestPlatformProductValidationProfile *
umi_test_platform_product_validation_profile_find(const char *product_id)
{
    size_t index;
    if (product_id == NULL) return NULL;
    for (index = 0U; index < umi_test_platform_product_validation_profile_count();
         ++index) {
        if (strcmp(PROFILES[index].product_id, product_id) == 0)
            return &PROFILES[index];
    }
    return NULL;
}
