/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/game/types.c
 *
 * PURPOSE:
 *   Implement safe defaults and human-readable names for game engine records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/game/types.h"

/* Construct an identity transform so new entities are visible and unscaled. */
UmiGameTransform umi_game_transform_identity(void)
{
    UmiGameTransform transform = {0};
    /* Scale uses one because zero would collapse the entity on every axis. */
    transform.scale_x = 1.0;
    transform.scale_y = 1.0;
    transform.scale_z = 1.0;
    return transform;
}

/* Convert a profile enumeration to stable non-allocated diagnostic text. */
const char *umi_game_world_profile_text(UmiGameWorldProfile profile)
{
    /* Unknown values remain diagnosable if a newer profile reaches this build. */
    switch (profile) {
        case UMI_GAME_WORLD_RETRO_2D: return "retro-2d";
        case UMI_GAME_WORLD_2D: return "2d";
        case UMI_GAME_WORLD_3D: return "3d";
        case UMI_GAME_WORLD_MIXED: return "mixed";
        default: return "unknown";
    }
}

/* Convert an entity kind to stable text for editors, logs and inspectors. */
const char *umi_game_entity_kind_text(UmiGameEntityKind kind)
{
    /* Broad labels keep the engine independent from a particular game genre. */
    switch (kind) {
        case UMI_GAME_ENTITY_ACTOR: return "actor";
        case UMI_GAME_ENTITY_ITEM: return "item";
        case UMI_GAME_ENTITY_ENVIRONMENT: return "environment";
        case UMI_GAME_ENTITY_TRIGGER: return "trigger";
        case UMI_GAME_ENTITY_CAMERA: return "camera";
        case UMI_GAME_ENTITY_LIGHT: return "light";
        case UMI_GAME_ENTITY_AUDIO: return "audio";
        case UMI_GAME_ENTITY_INTERFACE: return "interface";
        default: return "unknown";
    }
}
