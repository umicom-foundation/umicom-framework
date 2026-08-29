/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/profiles/kitchen-designer/standard.inc
 *
 * PURPOSE:
 *   Balance immediate validation, automatic drafts and clear everyday application feedback.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationExperienceProfile KITCHEN_DESIGNER_STANDARD_EXPERIENCE_PROFILE = {
    sizeof(UmiApplicationExperienceProfile),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.experience.kitchen-designer.standard",
    "org.umicom.workspace.kitchen-designer.standard",
    "org.umicom.kitchen-designer",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD,
    "umicom.command.plan.render",
    "umicom.form.plan.settings",
    {1, 1, 1, 48U},
    {UMI_APPLICATION_EXPERIENCE_VALIDATE_ON_CHANGE, 1, 1, 1},
    {UMI_APPLICATION_EXPERIENCE_DENSITY_COMFORTABLE, 1, 1, 6U}};
