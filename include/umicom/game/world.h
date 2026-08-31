/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/game/world.h
 *
 * PURPOSE:
 *   Own a bounded game entity world whose stable numeric identities can be
 *   shared by role-playing, adventure, multiplayer and retro-style games.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_GAME_WORLD_H
#define UMICOM_GAME_WORLD_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/game/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The opaque world owns all entity storage and hides allocation details. */
typedef struct UmiGameWorld UmiGameWorld;

/* A snapshot exposes counts and revision without exposing owned memory. */
typedef struct UmiGameWorldSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiGameWorldProfile profile;
    size_t entity_count;
    size_t entity_capacity;
    uint64_t revision;
} UmiGameWorldSnapshot;

/* Create a bounded world; explicit capacity makes memory use predictable. */
UmiStatus umi_game_world_create(
    UmiGameWorldProfile profile,
    size_t entity_capacity,
    UmiGameWorld **out_world);

/* Destroy the world and every entity record it owns. */
void umi_game_world_destroy(UmiGameWorld *world);

/* Spawn an entity and return a stable non-zero identity. */
UmiStatus umi_game_world_spawn(
    UmiGameWorld *world,
    UmiGameEntityKind kind,
    const char *name,
    UmiGameEntityId *out_entity_id);

/* Remove an entity while invalidating only that identity. */
UmiStatus umi_game_world_despawn(
    UmiGameWorld *world,
    UmiGameEntityId entity_id);

/* Replace an entity transform after validating non-zero scale. */
UmiStatus umi_game_world_set_transform(
    UmiGameWorld *world,
    UmiGameEntityId entity_id,
    const UmiGameTransform *transform);

/* Store application-defined tag bits without changing engine ownership. */
UmiStatus umi_game_world_set_tags(
    UmiGameWorld *world,
    UmiGameEntityId entity_id,
    uint64_t tags);

/* Copy one entity by identity into caller-owned storage. */
UmiStatus umi_game_world_find(
    const UmiGameWorld *world,
    UmiGameEntityId entity_id,
    UmiGameEntity *out_entity);

/* Copy the entity at one active enumeration position. */
UmiStatus umi_game_world_at(
    const UmiGameWorld *world,
    size_t index,
    UmiGameEntity *out_entity);

/* Copy world counts and revision for UI and persistence decisions. */
UmiStatus umi_game_world_snapshot(
    const UmiGameWorld *world,
    UmiGameWorldSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
