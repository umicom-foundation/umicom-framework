/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/profiles/database-studio/focus.inc
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
static const UmiApplicationExperienceProfile DATABASE_STUDIO_FOCUS_EXPERIENCE_PROFILE = {
    sizeof(UmiApplicationExperienceProfile),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.experience.database-studio.focus",
    "org.umicom.workspace.database-studio.focus",
    "org.umicom.database-studio",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    "umicom.command.query.run",
    "umicom.form.query.editor",
    {1, 1, 1, 64U},
    {UMI_APPLICATION_EXPERIENCE_VALIDATE_ON_SUBMIT, 1, 0, 1},
    {UMI_APPLICATION_EXPERIENCE_DENSITY_COMPACT, 0, 1, 3U}};
