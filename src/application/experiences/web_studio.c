/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experiences/web_studio.c
 *
 * PURPOSE:
 *   Define the canonical Framework-owned product experience for Umicom Web Studio.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experiences/web_studio.h"

#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

/*
 * Web Studio separates visual page design from testing and inspection.
 * Canvas, document, preview and diagnostic panels remain Framework components
 * that can be rearranged without embedding web tooling in the application. */

static const UmiExperiencePanelDefinition PANELS[] = {
    { sizeof(UmiExperiencePanelDefinition), "canvas", "Web Designer",
      "Responsive visual page designer", "umicom.web", "development.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "components", "Components",
      "Reusable web components", "umicom.designer", "development.blue", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "code", "Code",
      "HTML/CSS/JS source", "umicom.editor", "development.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR },
    { sizeof(UmiExperiencePanelDefinition), "properties", "Properties",
      "Layout/style/data binding properties", "umicom.designer", "development.blue", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "preview", "Preview",
      "Responsive live preview", "umicom.web", "development.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "browser", "Browser",
      "Integrated browser/test surface", "umicom.web", "development.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "problems", "Problems",
      "Web diagnostics", "umicom.diagnostics", "operations.green", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE },
    { sizeof(UmiExperiencePanelDefinition), "network", "Network",
      "Request/response diagnostics", "umicom.web", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "accessibility", "Accessibility",
      "Accessibility audit", "umicom.ui", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE }
};

static const char *const LAYOUT_DESIGN[] = {"canvas", "components", "code", "properties", "preview"};
static const char *const LAYOUT_TEST[] = {"preview", "browser", "problems", "network", "accessibility"};

static const UmiExperienceLayoutDefinition LAYOUTS[] = {
    { sizeof(UmiExperienceLayoutDefinition), "design", "Design",
      "Visual/code-first web design", LAYOUT_DESIGN, COUNT_OF(LAYOUT_DESIGN), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR | UMI_EXPERIENCE_LAYOUT_RESPONSIVE },
    { sizeof(UmiExperienceLayoutDefinition), "test", "Test",
      "Browser, API and accessibility testing", LAYOUT_TEST, COUNT_OF(LAYOUT_TEST), UMI_EXPERIENCE_LAYOUT_LOCKABLE }
};

static const UmiExperienceFeatureDefinition FEATURES[] = {
    { sizeof(UmiExperienceFeatureDefinition), "web.visual", "Visual designer",
      "Responsive components/layout", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.designer" },
    { sizeof(UmiExperienceFeatureDefinition), "web.code", "Web code",
      "HTML/CSS/JS/TS editing and intelligence", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.editor" },
    { sizeof(UmiExperienceFeatureDefinition), "web.preview", "Preview",
      "Live responsive preview", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.web" },
    { sizeof(UmiExperienceFeatureDefinition), "web.test", "Testing",
      "Browser/network/accessibility tests", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.web" },
    { sizeof(UmiExperienceFeatureDefinition), "web.deploy", "Deployment",
      "Build/package/publish", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.delivery" }
};

static const UmiApplicationExperienceDefinition DEFINITION = {
    sizeof(UmiApplicationExperienceDefinition),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.web-studio",
    "Umicom Web Studio",
    "design",
    PANELS,
    COUNT_OF(PANELS),
    LAYOUTS,
    COUNT_OF(LAYOUTS),
    FEATURES,
    COUNT_OF(FEATURES)
};

/*
 * Provide the application experience web studio operation used by this module and its
 * client applications.
 */
const UmiApplicationExperienceDefinition *umi_application_experience_web_studio(void)
{
    return &DEFINITION;
}
