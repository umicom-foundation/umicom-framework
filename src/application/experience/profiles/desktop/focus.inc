/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/profiles/desktop/focus.inc
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
static const UmiApplicationExperienceProfile DESKTOP_FOCUS_EXPERIENCE_PROFILE = {
    sizeof(UmiApplicationExperienceProfile),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.experience.desktop.focus",
    "org.umicom.workspace.desktop.focus",
    "org.umicom.desktop",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    "umicom.command.application.launch",
    "umicom.form.application.launcher",
    {1, 1, 1, 64U},
    {UMI_APPLICATION_EXPERIENCE_VALIDATE_ON_SUBMIT, 1, 0, 1},
    {UMI_APPLICATION_EXPERIENCE_DENSITY_COMPACT, 0, 1, 3U}};
