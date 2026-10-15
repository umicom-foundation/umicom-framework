/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/profiles/healthcare/learning.inc
 * PURPOSE: Define the synthetic request form policy for a portable Healthcare workspace.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Clinical authentication and persistent records are not provided by this
 * presentation profile. The native exercise keeps its records in session memory. */
static const UmiApplicationExperienceProfile HEALTHCARE_LEARNING_EXPERIENCE_PROFILE = {
    sizeof(UmiApplicationExperienceProfile),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.experience.healthcare.learning",
    "org.umicom.workspace.healthcare.learning",
    "org.umicom.healthcare",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    "umicom.command.healthcare.request",
    "umicom.form.healthcare.request",
    {1, 1, 1, 32U},
    {UMI_APPLICATION_EXPERIENCE_VALIDATE_ON_BLUR, 0, 1, 1},
    {UMI_APPLICATION_EXPERIENCE_DENSITY_COMFORTABLE, 1, 1, 4U}};
