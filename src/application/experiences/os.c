/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experiences/os.c
 *
 * PURPOSE:
 *   Define the canonical Framework-owned product experience for Umicom OS Control Centre.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experiences/os.h"

#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

static const UmiExperiencePanelDefinition PANELS[] = {
    { sizeof(UmiExperiencePanelDefinition), "system-info", "System Information",
      "CPU, memory, GPU and OS profile", "umicom.os", "system.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "devices", "Devices",
      "Hardware/device inventory", "umicom.os", "system.blue", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "storage", "Storage",
      "Disks, filesystems and volumes", "umicom.os", "system.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "network", "Network",
      "Interfaces, Wi-Fi and connectivity", "umicom.os", "system.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "services", "Services",
      "System/user-space service state", "umicom.os", "operations.green", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "processes", "Processes",
      "Process/runtime monitoring", "umicom.process", "operations.green", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "security", "Security",
      "User-space security posture", "umicom.security", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CRITICAL },
    { sizeof(UmiExperiencePanelDefinition), "updates", "Updates",
      "Package/update status", "umicom.delivery", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "logs", "Logs",
      "System/application logs", "umicom.diagnostics", "operations.green", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "developer-options", "Developer Options",
      "Runtime and developer controls", "umicom.os", "system.blue", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE }
};

static const char *const LAYOUT_SYSTEM[] = {"system-info", "devices", "storage", "network", "services", "security", "updates"};
static const char *const LAYOUT_DEVELOPER[] = {"system-info", "services", "processes", "logs", "developer-options"};

static const UmiExperienceLayoutDefinition LAYOUTS[] = {
    { sizeof(UmiExperienceLayoutDefinition), "system", "System",
      "System information and controls", LAYOUT_SYSTEM, COUNT_OF(LAYOUT_SYSTEM), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_RESPONSIVE },
    { sizeof(UmiExperienceLayoutDefinition), "developer", "Developer",
      "Runtime, services and developer system controls", LAYOUT_DEVELOPER, COUNT_OF(LAYOUT_DEVELOPER), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR }
};

static const UmiExperienceFeatureDefinition FEATURES[] = {
    { sizeof(UmiExperienceFeatureDefinition), "os.control-centre", "Control Centre",
      "Portable user-space system views", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.os" },
    { sizeof(UmiExperienceFeatureDefinition), "os.system-info", "System information",
      "Hardware/runtime inventory", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.os" },
    { sizeof(UmiExperienceFeatureDefinition), "os.services", "Service/process controls",
      "User-space services and processes", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.os" },
    { sizeof(UmiExperienceFeatureDefinition), "os.updates", "Updates",
      "Package/update UI boundaries", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_EXTERNAL_ADAPTER, "umicom.delivery" },
    { sizeof(UmiExperienceFeatureDefinition), "os.distribution", "OS distribution",
      "Kernel/boot/rootfs/recovery outside Framework", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P4,
      UMI_EXPERIENCE_OWNER_EXTERNAL_ADAPTER, "umicom.os.distribution" }
};

static const UmiApplicationExperienceDefinition DEFINITION = {
    sizeof(UmiApplicationExperienceDefinition),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.os",
    "Umicom OS Control Centre",
    "system",
    PANELS,
    COUNT_OF(PANELS),
    LAYOUTS,
    COUNT_OF(LAYOUTS),
    FEATURES,
    COUNT_OF(FEATURES)
};

const UmiApplicationExperienceDefinition *umi_application_experience_os(void)
{
    return &DEFINITION;
}
