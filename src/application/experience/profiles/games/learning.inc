/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/profiles/games/learning.inc
 *
 * PURPOSE:
 *   Make commands, forms and recovery guidance explicit for someone learning the application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationExperienceProfile GAMES_LEARNING_EXPERIENCE_PROFILE = {
    sizeof(UmiApplicationExperienceProfile),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.experience.games.learning",
    "org.umicom.workspace.games.learning",
    "org.umicom.games",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    "umicom.command.simulation.play",
    "umicom.form.simulation.settings",
    {1, 1, 1, 32U},
    {UMI_APPLICATION_EXPERIENCE_VALIDATE_ON_BLUR, 0, 1, 1},
    {UMI_APPLICATION_EXPERIENCE_DENSITY_COMFORTABLE, 1, 1, 4U}};
