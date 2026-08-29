/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/profiles/accountant/learning.inc
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
static const UmiApplicationExperienceProfile ACCOUNTANT_LEARNING_EXPERIENCE_PROFILE = {
    sizeof(UmiApplicationExperienceProfile),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.experience.accountant.learning",
    "org.umicom.workspace.accountant.learning",
    "org.umicom.accountant",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    "umicom.command.journal.post",
    "umicom.form.journal.entry",
    {1, 1, 1, 32U},
    {UMI_APPLICATION_EXPERIENCE_VALIDATE_ON_BLUR, 0, 1, 1},
    {UMI_APPLICATION_EXPERIENCE_DENSITY_COMFORTABLE, 1, 1, 4U}};
