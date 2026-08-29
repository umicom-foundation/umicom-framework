/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/security-centre/standard.inc
 *
 * PURPOSE:
 *   Define the standard workspace recipe for Umicom Security Centre using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot SECURITY_CENTRE_STANDARD_SLOTS[] = {
    {"umicom.shared.security", "umicom.shared.security", UMI_APPLICATION_COMPONENT_REGION_PRIMARY,
     6U, 1, 1},
    {"umicom.security.identities", "umicom.security.identities",
     UMI_APPLICATION_COMPONENT_REGION_LEFT, 2U, 1, 0},
    {"umicom.security.policy", "umicom.security.policy", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 2U,
     1, 0},
    {"umicom.shared.audit", "umicom.shared.audit", UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 2U, 1,
     0},
    {"umicom.shared.surveillance", "umicom.shared.surveillance",
     UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1, 0},
    {"umicom.shared.tasks", "umicom.shared.tasks", UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
};

static const UmiApplicationComponentRecipe SECURITY_CENTRE_STANDARD_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.security-centre.standard",
    "org.umicom.security-centre",
    "Umicom Security Centre Standard",
    "The balanced everyday Umicom Security Centre workspace with core creation, inspection and "
    "supporting tools.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD,
    SECURITY_CENTRE_STANDARD_SLOTS,
    sizeof(SECURITY_CENTRE_STANDARD_SLOTS) / sizeof(SECURITY_CENTRE_STANDARD_SLOTS[0])};
