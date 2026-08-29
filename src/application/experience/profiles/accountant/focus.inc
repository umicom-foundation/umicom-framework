/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/profiles/accountant/focus.inc
 *
 * PURPOSE:
 *   d quiet while preserving confirmation, undo and accessible feedback.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationExperienceProfile ACCOUNTANT_FOCUS_EXPERIENCE_PROFILE = {
    sizeof(UmiApplicationExperienceProfile),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.experience.accountant.focus",
    "org.umicom.workspace.accountant.focus",
    "org.umicom.accountant",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    "umicom.command.journal.post",
    "umicom.form.journal.entry",
    {1, 1, 1, 64U},
    {UMI_APPLICATION_EXPERIENCE_VALIDATE_ON_SUBMIT, 1, 0, 1},
    {UMI_APPLICATION_EXPERIENCE_DENSITY_COMPACT, 0, 1, 3U}};
