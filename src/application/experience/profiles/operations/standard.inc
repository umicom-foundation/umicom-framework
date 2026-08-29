/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/profiles/operations/standard.inc
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
static const UmiApplicationExperienceProfile OPERATIONS_STANDARD_EXPERIENCE_PROFILE = {
    sizeof(UmiApplicationExperienceProfile),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.experience.operations.standard",
    "org.umicom.workspace.operations.standard",
    "org.umicom.operations",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD,
    "umicom.command.incident.create",
    "umicom.form.incident.record",
    {1, 1, 1, 48U},
    {UMI_APPLICATION_EXPERIENCE_VALIDATE_ON_CHANGE, 1, 1, 1},
    {UMI_APPLICATION_EXPERIENCE_DENSITY_COMFORTABLE, 1, 1, 6U}};
