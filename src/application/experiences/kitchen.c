/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experiences/kitchen.c
 *
 * PURPOSE:
 *   Define the canonical Framework-owned product experience for Umicom Kitchen Designer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experiences/kitchen.h"

#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

static const UmiExperiencePanelDefinition PANELS[] = {
    { sizeof(UmiExperiencePanelDefinition), "canvas", "Design Canvas",
      "2D/3D kitchen layout", "umicom.designer", "design.teal", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "catalogue", "Catalogue",
      "Cabinets, appliances and products", "umicom.designer", "design.teal", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "properties", "Properties",
      "Dimensions, constraints and options", "umicom.designer", "design.teal", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "costing", "Costing",
      "BOM and pricing", "umicom.data", "finance.blue", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "render", "Render",
      "Photorealistic presentation", "umicom.media", "design.teal", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "materials", "Materials",
      "Finishes and materials", "umicom.media", "design.teal", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE }
};

static const char *const LAYOUT_DESIGN[] = {"canvas", "catalogue", "properties", "costing"};
static const char *const LAYOUT_PRESENT[] = {"render", "materials", "costing"};

static const UmiExperienceLayoutDefinition LAYOUTS[] = {
    { sizeof(UmiExperienceLayoutDefinition), "design", "Design",
      "Room/cabinet planning and properties", LAYOUT_DESIGN, COUNT_OF(LAYOUT_DESIGN), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR },
    { sizeof(UmiExperienceLayoutDefinition), "present", "Present",
      "Rendering and customer presentation", LAYOUT_PRESENT, COUNT_OF(LAYOUT_PRESENT), UMI_EXPERIENCE_LAYOUT_LOCKABLE }
};

static const UmiExperienceFeatureDefinition FEATURES[] = {
    { sizeof(UmiExperienceFeatureDefinition), "kitchen.parametric", "Parametric model",
      "Room/cabinet constraints and catalogue", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.designer" },
    { sizeof(UmiExperienceFeatureDefinition), "kitchen.cost", "Costing",
      "BOM, pricing and quoting", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.data" },
    { sizeof(UmiExperienceFeatureDefinition), "kitchen.render", "Rendering",
      "Materials and presentation", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P3,
      UMI_EXPERIENCE_OWNER_EXTERNAL_ADAPTER, "umicom.media" },
    { sizeof(UmiExperienceFeatureDefinition), "kitchen.ai", "AI design",
      "Design suggestions and alternatives", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P4,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.ai" }
};

static const UmiApplicationExperienceDefinition DEFINITION = {
    sizeof(UmiApplicationExperienceDefinition),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.kitchen-designer",
    "Umicom Kitchen Designer",
    "design",
    PANELS,
    COUNT_OF(PANELS),
    LAYOUTS,
    COUNT_OF(LAYOUTS),
    FEATURES,
    COUNT_OF(FEATURES)
};

const UmiApplicationExperienceDefinition *umi_application_experience_kitchen(void)
{
    return &DEFINITION;
}
