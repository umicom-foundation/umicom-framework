/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/profiles/kitchen-designer/focus.inc
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
static const UmiApplicationExperienceProfile KITCHEN_DESIGNER_FOCUS_EXPERIENCE_PROFILE = {
    sizeof(UmiApplicationExperienceProfile),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.experience.kitchen-designer.focus",
    "org.umicom.workspace.kitchen-designer.focus",
    "org.umicom.kitchen-designer",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    "umicom.command.plan.render",
    "umicom.form.plan.settings",
    {1, 1, 1, 64U},
    {UMI_APPLICATION_EXPERIENCE_VALIDATE_ON_SUBMIT, 1, 0, 1},
    {UMI_APPLICATION_EXPERIENCE_DENSITY_COMPACT, 0, 1, 3U}};
