/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experiences/desktop.c
 *
 * PURPOSE:
 *   Define the canonical Framework-owned product experience for Umicom Desk.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experiences/desktop.h"

#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

static const UmiExperiencePanelDefinition PANELS[] = {
    { sizeof(UmiExperiencePanelDefinition), "application-centre", "Application Centre",
      "Applications, files, settings and commands", "umicom.application.federation", "system.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "taskbar", "Taskbar",
      "Validated runtime applications", "umicom.runtime", "system.blue", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE },
    { sizeof(UmiExperiencePanelDefinition), "workspace", "Workspace",
      "Windows and cross-app mosaic", "umicom.ui", "system.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "notifications", "Notifications",
      "System and application notifications", "umicom.ui", "system.blue", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE },
    { sizeof(UmiExperiencePanelDefinition), "quick-settings", "Quick Settings",
      "Network, audio, display and session controls", "umicom.os", "system.blue", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "global-search", "Global Search",
      "Cross-application search and commands", "umicom.application.federation", "system.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "operations", "Operations",
      "Fleet/runtime health", "umicom.operations", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "context", "Context",
      "Cross-app linked selection/context", "umicom.application.federation", "system.blue", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED }
};

static const char *const LAYOUT_FLOW[] = {"application-centre", "taskbar", "workspace", "notifications", "quick-settings"};
static const char *const LAYOUT_WORKSTATION[] = {"taskbar", "workspace", "global-search", "operations", "context"};

static const UmiExperienceLayoutDefinition LAYOUTS[] = {
    { sizeof(UmiExperienceLayoutDefinition), "flow", "Flow",
      "Modern default desktop", LAYOUT_FLOW, COUNT_OF(LAYOUT_FLOW), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR | UMI_EXPERIENCE_LAYOUT_RESPONSIVE },
    { sizeof(UmiExperienceLayoutDefinition), "workstation", "Workstation",
      "Power-user multi-monitor suite workspace", LAYOUT_WORKSTATION, COUNT_OF(LAYOUT_WORKSTATION), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR | UMI_EXPERIENCE_LAYOUT_CONTEXT_LINKED }
};

static const UmiExperienceFeatureDefinition FEATURES[] = {
    { sizeof(UmiExperienceFeatureDefinition), "desktop.launcher", "Launcher/taskbar",
      "Application discovery and process supervision", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.application.federation" },
    { sizeof(UmiExperienceFeatureDefinition), "desktop.layouts", "Desktop layouts",
      "Semantic multi-monitor layouts", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.ui" },
    { sizeof(UmiExperienceFeatureDefinition), "desktop.mosaic", "Cross-app mosaic",
      "Federated application content composition", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.application.federation" },
    { sizeof(UmiExperienceFeatureDefinition), "desktop.session", "Session restore",
      "Persisted application/layout recovery", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.data" },
    { sizeof(UmiExperienceFeatureDefinition), "desktop.global-search", "Global search/commands",
      "Suite-wide discovery and actions", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.application.federation" }
};

static const UmiApplicationExperienceDefinition DEFINITION = {
    sizeof(UmiApplicationExperienceDefinition),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.desktop",
    "Umicom Desk",
    "flow",
    PANELS,
    COUNT_OF(PANELS),
    LAYOUTS,
    COUNT_OF(LAYOUTS),
    FEATURES,
    COUNT_OF(FEATURES)
};

const UmiApplicationExperienceDefinition *umi_application_experience_desktop(void)
{
    return &DEFINITION;
}
