/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experiences/cad.c
 *
 * PURPOSE:
 *   Define the canonical Framework-owned product experience for Umicom CAD.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experiences/cad.h"

#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

static const UmiExperiencePanelDefinition PANELS[] = {
    { sizeof(UmiExperiencePanelDefinition), "canvas", "CAD Canvas",
      "2D/3D modelling surface", "umicom.cad", "design.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "tree", "Model Tree",
      "Feature/assembly hierarchy", "umicom.cad", "design.blue", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "properties", "Properties",
      "Geometry/object properties", "umicom.cad", "design.blue", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "constraints", "Constraints",
      "Parametric constraints", "umicom.cad", "design.blue", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "layers", "Layers",
      "Layer/visibility management", "umicom.cad", "design.blue", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "inspector", "Inspector",
      "Model interrogation", "umicom.cad", "design.blue", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "measurements", "Measurements",
      "Distance/angle/area evidence", "umicom.cad", "design.blue", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE }
};

static const char *const LAYOUT_DESIGN[] = {"canvas", "tree", "properties", "constraints", "layers"};
static const char *const LAYOUT_INSPECT[] = {"canvas", "inspector", "measurements", "properties"};

static const UmiExperienceLayoutDefinition LAYOUTS[] = {
    { sizeof(UmiExperienceLayoutDefinition), "design", "Design",
      "2D/3D parametric design", LAYOUT_DESIGN, COUNT_OF(LAYOUT_DESIGN), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR },
    { sizeof(UmiExperienceLayoutDefinition), "inspect", "Inspect",
      "Measurements and model inspection", LAYOUT_INSPECT, COUNT_OF(LAYOUT_INSPECT), UMI_EXPERIENCE_LAYOUT_LOCKABLE }
};

static const UmiExperienceFeatureDefinition FEATURES[] = {
    { sizeof(UmiExperienceFeatureDefinition), "cad.2d", "2D drafting",
      "Geometry, snapping, constraints and layers", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.cad" },
    { sizeof(UmiExperienceFeatureDefinition), "cad.3d", "3D modelling",
      "Solids, features and assemblies", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P3,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.cad" },
    { sizeof(UmiExperienceFeatureDefinition), "cad.io", "Import/export",
      "Provider-neutral CAD formats", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P3,
      UMI_EXPERIENCE_OWNER_EXTERNAL_ADAPTER, "umicom.integration" },
    { sizeof(UmiExperienceFeatureDefinition), "cad.render", "Rendering",
      "Shaded/rendered presentation", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P4,
      UMI_EXPERIENCE_OWNER_EXTERNAL_ADAPTER, "umicom.media" }
};

static const UmiApplicationExperienceDefinition DEFINITION = {
    sizeof(UmiApplicationExperienceDefinition),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.cad",
    "Umicom CAD",
    "design",
    PANELS,
    COUNT_OF(PANELS),
    LAYOUTS,
    COUNT_OF(LAYOUTS),
    FEATURES,
    COUNT_OF(FEATURES)
};

const UmiApplicationExperienceDefinition *umi_application_experience_cad(void)
{
    return &DEFINITION;
}
