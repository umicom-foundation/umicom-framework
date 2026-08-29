/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/security-centre/focus.inc
 *
 * PURPOSE:
 *   Define the focus workspace recipe for Umicom Security Centre using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot SECURITY_CENTRE_FOCUS_SLOTS[] = {
    {"umicom.shared.security", "umicom.shared.security", UMI_APPLICATION_COMPONENT_REGION_PRIMARY,
     7U, 1, 1},
    {"umicom.security.policy", "umicom.security.policy", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 2U,
     1, 0},
    {"umicom.shared.audit", "umicom.shared.audit", UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1,
     0},
};

static const UmiApplicationComponentRecipe SECURITY_CENTRE_FOCUS_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.security-centre.focus",
    "org.umicom.security-centre",
    "Umicom Security Centre Focus",
    "A distraction-reduced Umicom Security Centre layout centred on the main task and its closest "
    "context.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    SECURITY_CENTRE_FOCUS_SLOTS,
    sizeof(SECURITY_CENTRE_FOCUS_SLOTS) / sizeof(SECURITY_CENTRE_FOCUS_SLOTS[0])};
