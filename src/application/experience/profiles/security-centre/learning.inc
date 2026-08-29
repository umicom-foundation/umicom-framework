/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/profiles/security-centre/learning.inc
 *
 * PURPOSE:
 *   Make commands, forms and recovery guidance explicit for someone learning the application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationExperienceProfile SECURITY_CENTRE_LEARNING_EXPERIENCE_PROFILE = {
    sizeof(UmiApplicationExperienceProfile),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.experience.security-centre.learning",
    "org.umicom.workspace.security-centre.learning",
    "org.umicom.security-centre",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    "umicom.command.access.review",
    "umicom.form.access.request",
    {1, 1, 1, 32U},
    {UMI_APPLICATION_EXPERIENCE_VALIDATE_ON_BLUR, 0, 1, 1},
    {UMI_APPLICATION_EXPERIENCE_DENSITY_COMFORTABLE, 1, 1, 4U}};
