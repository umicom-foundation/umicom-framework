/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/profiles/studio/learning.inc
 *
 * PURPOSE:
 *   and recovery guidance explicit for someone learning the application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationExperienceProfile STUDIO_LEARNING_EXPERIENCE_PROFILE = {
    sizeof(UmiApplicationExperienceProfile),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.experience.studio.learning",
    "org.umicom.workspace.studio.learning",
    "org.umicom.studio",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    "umicom.command.project.run",
    "umicom.form.project.settings",
    {1, 1, 1, 32U},
    {UMI_APPLICATION_EXPERIENCE_VALIDATE_ON_BLUR, 0, 1, 1},
    {UMI_APPLICATION_EXPERIENCE_DENSITY_COMFORTABLE, 1, 1, 4U}};
