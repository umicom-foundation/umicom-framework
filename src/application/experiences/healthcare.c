/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experiences/healthcare.c
 * PURPOSE: Describe the synthetic Healthcare host for shared launch readiness.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experiences/healthcare.h"
#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

/* The launcher requires a real default experience, not just an executable name.
 * Restrict this description to the synthetic workspace actually hosted today. */
static const UmiExperiencePanelDefinition PANELS[] = {
    {sizeof(UmiExperiencePanelDefinition), "access", "Access requests",
     "Synthetic requests, provider replies and collection", "umicom.ui",
     "platform.blue", "centre", UMI_EXPERIENCE_PANEL_DOCKABLE}
};
static const char *const ACCESS_PANELS[] = {"access"};
static const UmiExperienceLayoutDefinition LAYOUTS[] = {
    {sizeof(UmiExperienceLayoutDefinition), "healthcare", "Healthcare",
     "Synthetic access coordination", ACCESS_PANELS, COUNT_OF(ACCESS_PANELS),
     UMI_EXPERIENCE_LAYOUT_LOCKABLE}
};
static const UmiExperienceFeatureDefinition FEATURES[] = {
    {sizeof(UmiExperienceFeatureDefinition), "healthcare.synthetic-access",
     "Synthetic access workflow", "In-memory synthetic access coordination",
     UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
     UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.ui"}
};
static const UmiApplicationExperienceDefinition DEFINITION = {
    sizeof(UmiApplicationExperienceDefinition), UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.healthcare", "Umicom Healthcare", "healthcare",
    PANELS, COUNT_OF(PANELS), LAYOUTS, COUNT_OF(LAYOUTS), FEATURES, COUNT_OF(FEATURES)
};

const UmiApplicationExperienceDefinition *umi_application_experience_healthcare(void)
{
    /* Static metadata is borrowed; callers never allocate or free it. */
    return &DEFINITION;
}
