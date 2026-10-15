/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/profiles/healthcare/focus.inc
 * PURPOSE: Define the synthetic request form policy for a portable Healthcare workspace.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Clinical authentication and persistent records are not provided by this
 * presentation profile. The native exercise keeps its records in session memory. */
static const UmiApplicationExperienceProfile HEALTHCARE_FOCUS_EXPERIENCE_PROFILE = {
    sizeof(UmiApplicationExperienceProfile),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.experience.healthcare.focus",
    "org.umicom.workspace.healthcare.focus",
    "org.umicom.healthcare",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    "umicom.command.healthcare.request",
    "umicom.form.healthcare.request",
    {1, 1, 1, 64U},
    {UMI_APPLICATION_EXPERIENCE_VALIDATE_ON_SUBMIT, 1, 0, 1},
    {UMI_APPLICATION_EXPERIENCE_DENSITY_COMPACT, 0, 1, 3U}};
