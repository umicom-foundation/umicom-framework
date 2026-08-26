/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experiences/mobile_studio.c
 *
 * PURPOSE:
 *   Define the canonical Framework-owned product experience for Umicom Mobile Studio.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experiences/mobile_studio.h"

#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

static const UmiExperiencePanelDefinition PANELS[] = {
    { sizeof(UmiExperiencePanelDefinition), "device-canvas", "Device Canvas",
      "Phone/tablet responsive designer", "umicom.frontend", "development.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "components", "Components",
      "Mobile UI components", "umicom.designer", "development.blue", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "properties", "Properties",
      "Responsive/device properties", "umicom.designer", "development.blue", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE },
    { sizeof(UmiExperiencePanelDefinition), "code", "Code",
      "Application source", "umicom.editor", "development.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "preview", "Preview",
      "Device preview", "umicom.frontend", "development.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "devices", "Devices",
      "Connected/emulated devices", "umicom.frontend", "operations.green", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "tests", "Tests",
      "Device/application tests", "umicom.testing", "operations.green", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "logs", "Logs",
      "Device/runtime output", "umicom.diagnostics", "operations.green", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE },
    { sizeof(UmiExperiencePanelDefinition), "delivery", "Delivery",
      "Build/sign/package/deploy", "umicom.delivery", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE }
};

static const char *const LAYOUT_DESIGN[] = {"device-canvas", "components", "properties", "code", "preview"};
static const char *const LAYOUT_TEST[] = {"devices", "preview", "tests", "logs", "delivery"};

static const UmiExperienceLayoutDefinition LAYOUTS[] = {
    { sizeof(UmiExperienceLayoutDefinition), "design", "Design",
      "Responsive mobile application design", LAYOUT_DESIGN, COUNT_OF(LAYOUT_DESIGN), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_RESPONSIVE },
    { sizeof(UmiExperienceLayoutDefinition), "test", "Test",
      "Devices, tests and deployment", LAYOUT_TEST, COUNT_OF(LAYOUT_TEST), UMI_EXPERIENCE_LAYOUT_LOCKABLE }
};

static const UmiExperienceFeatureDefinition FEATURES[] = {
    { sizeof(UmiExperienceFeatureDefinition), "mobile.design", "Mobile designer",
      "Responsive UI/device profiles", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.frontend" },
    { sizeof(UmiExperienceFeatureDefinition), "mobile.build", "Build",
      "Multi-platform build/sign/package", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.build" },
    { sizeof(UmiExperienceFeatureDefinition), "mobile.test", "Device testing",
      "Emulator/device test workflows", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P3,
      UMI_EXPERIENCE_OWNER_EXTERNAL_ADAPTER, "umicom.testing" },
    { sizeof(UmiExperienceFeatureDefinition), "mobile.delivery", "Store/release",
      "Deployment and release workflow", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P3,
      UMI_EXPERIENCE_OWNER_EXTERNAL_ADAPTER, "umicom.delivery" }
};

static const UmiApplicationExperienceDefinition DEFINITION = {
    sizeof(UmiApplicationExperienceDefinition),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.mobile-studio",
    "Umicom Mobile Studio",
    "design",
    PANELS,
    COUNT_OF(PANELS),
    LAYOUTS,
    COUNT_OF(LAYOUTS),
    FEATURES,
    COUNT_OF(FEATURES)
};

const UmiApplicationExperienceDefinition *umi_application_experience_mobile_studio(void)
{
    return &DEFINITION;
}
