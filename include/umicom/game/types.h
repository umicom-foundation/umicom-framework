/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/game/types.h
 *
 * PURPOSE:
 *   Define toolkit-neutral game identities, entity categories and transforms
 *   shared by two-dimensional, three-dimensional and retro-style games.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_GAME_TYPES_H
#define UMICOM_GAME_TYPES_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_GAME_NAME_CAPACITY 64U
#define UMI_GAME_PARTICIPANT_ID_CAPACITY 64U

/* Numeric identities stay valid when internal storage moves or is rebuilt. */
typedef uint64_t UmiGameEntityId;

/* A world profile lets one engine serve pixel, 2D, 3D and mixed experiences. */
typedef enum UmiGameWorldProfile {
    UMI_GAME_WORLD_RETRO_2D = 0,
    UMI_GAME_WORLD_2D = 1,
    UMI_GAME_WORLD_3D = 2,
    UMI_GAME_WORLD_MIXED = 3
} UmiGameWorldProfile;

/* Entity kinds are broad building blocks rather than game-specific classes. */
typedef enum UmiGameEntityKind {
    UMI_GAME_ENTITY_ACTOR = 0,
    UMI_GAME_ENTITY_ITEM = 1,
    UMI_GAME_ENTITY_ENVIRONMENT = 2,
    UMI_GAME_ENTITY_TRIGGER = 3,
    UMI_GAME_ENTITY_CAMERA = 4,
    UMI_GAME_ENTITY_LIGHT = 5,
    UMI_GAME_ENTITY_AUDIO = 6,
    UMI_GAME_ENTITY_INTERFACE = 7
} UmiGameEntityKind;

/* A plain transform is serialisable and contains no rendering-toolkit object. */
typedef struct UmiGameTransform {
    double position_x;
    double position_y;
    double position_z;
    double rotation_x;
    double rotation_y;
    double rotation_z;
    double scale_x;
    double scale_y;
    double scale_z;
} UmiGameTransform;

/* Entity snapshots are copies, preventing dangling pointers into the world. */
typedef struct UmiGameEntity {
    UmiGameEntityId entity_id;
    UmiGameEntityKind kind;
    char name[UMI_GAME_NAME_CAPACITY];
    UmiGameTransform transform;
    uint64_t tags;
    bool active;
} UmiGameEntity;

/* Create the neutral transform used by newly spawned entities. */
UmiGameTransform umi_game_transform_identity(void);

/* Return stable display text for a world profile. */
const char *umi_game_world_profile_text(UmiGameWorldProfile profile);

/* Return stable display text for an entity kind. */
const char *umi_game_entity_kind_text(UmiGameEntityKind kind);

#ifdef __cplusplus
}
#endif

#endif
