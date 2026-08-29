/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/profiles/mobile-studio/focus.inc
 *
 * PURPOSE:
 *   Keep the workspace compact and quiet while preserving confirmation, undo and accessible
 *feedback.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationExperienceProfile MOBILE_STUDIO_FOCUS_EXPERIENCE_PROFILE = {
    sizeof(UmiApplicationExperienceProfile),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.experience.mobile-studio.focus",
    "org.umicom.workspace.mobile-studio.focus",
    "org.umicom.mobile-studio",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    "umicom.command.device.run",
    "umicom.form.device.profile",
    {1, 1, 1, 64U},
    {UMI_APPLICATION_EXPERIENCE_VALIDATE_ON_SUBMIT, 1, 0, 1},
    {UMI_APPLICATION_EXPERIENCE_DENSITY_COMPACT, 0, 1, 3U}};
