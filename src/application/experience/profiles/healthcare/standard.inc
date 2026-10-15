/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/profiles/healthcare/standard.inc
 * PURPOSE: Define the synthetic request form policy for a portable Healthcare workspace.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Clinical authentication and persistent records are not provided by this
 * presentation profile. The native exercise keeps its records in session memory. */
static const UmiApplicationExperienceProfile HEALTHCARE_STANDARD_EXPERIENCE_PROFILE = {
    sizeof(UmiApplicationExperienceProfile),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.experience.healthcare.standard",
    "org.umicom.workspace.healthcare.standard",
    "org.umicom.healthcare",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD,
    "umicom.command.healthcare.request",
    "umicom.form.healthcare.request",
    {1, 1, 1, 48U},
    {UMI_APPLICATION_EXPERIENCE_VALIDATE_ON_CHANGE, 1, 1, 1},
    {UMI_APPLICATION_EXPERIENCE_DENSITY_COMFORTABLE, 1, 1, 6U}};
