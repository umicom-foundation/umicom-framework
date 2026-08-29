/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/security-centre/learning.inc
 *
 * PURPOSE:
 *   Define the learning workspace recipe for Umicom Security Centre using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot SECURITY_CENTRE_LEARNING_SLOTS[] = {
    {"umicom.shared.security", "umicom.shared.security", UMI_APPLICATION_COMPONENT_REGION_PRIMARY,
     4U, 1, 1},
    {"umicom.security.identities", "umicom.security.identities",
     UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
    {"umicom.security.policy", "umicom.security.policy", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U,
     1, 0},
};

static const UmiApplicationComponentRecipe SECURITY_CENTRE_LEARNING_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.security-centre.learning",
    "org.umicom.security-centre",
    "Umicom Security Centre Learning",
    "A calm starting layout for learning Umicom Security Centre with only the essential surfaces "
    "visible.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    SECURITY_CENTRE_LEARNING_SLOTS,
    sizeof(SECURITY_CENTRE_LEARNING_SLOTS) / sizeof(SECURITY_CENTRE_LEARNING_SLOTS[0]),
    "org.umicom.experience.security-centre.learning"};
