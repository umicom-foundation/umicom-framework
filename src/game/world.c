/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/game/world.c
 *
 * PURPOSE:
 *   Implement a bounded entity world with copied records and stable numeric
 *   identities so callers never retain pointers into mutable engine storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/game/world.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* World storage is private so allocation and compaction can evolve safely. */
struct UmiGameWorld {
    UmiGameWorldProfile profile;
    UmiGameEntity *entities;
    size_t capacity;
    size_t count;
    UmiGameEntityId next_entity_id;
    uint64_t revision;
};

/* Reject unknown persisted profile values before constructing a world. */
static bool valid_profile(UmiGameWorldProfile profile)
{
    return profile >= UMI_GAME_WORLD_RETRO_2D &&
        profile <= UMI_GAME_WORLD_MIXED;
}

/* Find mutable internal storage by stable identity; never return it publicly. */
static UmiGameEntity *find_mutable(
    UmiGameWorld *world,
    UmiGameEntityId entity_id)
{
    size_t index;
    /* Zero is reserved as "no entity" and cannot match an active record. */
    if (world == NULL || entity_id == 0U) return NULL;
    for (index = 0U; index < world->capacity; ++index) {
        /* Both active state and identity must match because slots are reused. */
        if (world->entities[index].active &&
            world->entities[index].entity_id == entity_id) {
            return &world->entities[index];
        }
    }
    return NULL;
}

/* Allocate a world and its fixed-capacity entity array as one ownership unit. */
UmiStatus umi_game_world_create(
    UmiGameWorldProfile profile,
    size_t entity_capacity,
    UmiGameWorld **out_world)
{
    UmiGameWorld *world;
    /* Multiplication is checked before calloc to prevent allocation overflow. */
    if (out_world == NULL || !valid_profile(profile) || entity_capacity == 0U ||
        entity_capacity > SIZE_MAX / sizeof(UmiGameEntity)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_world = NULL;
    world = (UmiGameWorld *)calloc(1U, sizeof(*world));
    /* Allocation failure leaves the caller's output predictably NULL. */
    if (world == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    world->entities = (UmiGameEntity *)calloc(
        entity_capacity, sizeof(*world->entities));
    /* Release the partially created owner when child allocation fails. */
    if (world->entities == NULL) {
        free(world);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    world->profile = profile;
    world->capacity = entity_capacity;
    world->next_entity_id = 1U;
    world->revision = 1U;
    *out_world = world;
    return UMI_STATUS_OK;
}

/* Release entity storage before the world owner; NULL destruction is safe. */
void umi_game_world_destroy(UmiGameWorld *world)
{
    if (world == NULL) return;
    free(world->entities);
    free(world);
}

/* Create one entity in the first free slot and assign a never-reused identity. */
UmiStatus umi_game_world_spawn(
    UmiGameWorld *world,
    UmiGameEntityKind kind,
    const char *name,
    UmiGameEntityId *out_entity_id)
{
    size_t index;
    int written;
    /* Empty names and unknown kinds would create records editors cannot explain. */
    if (world == NULL || name == NULL || name[0] == '\0' ||
        out_entity_id == NULL || kind < UMI_GAME_ENTITY_ACTOR ||
        kind > UMI_GAME_ENTITY_INTERFACE) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Capacity and identity overflow are reported before scanning storage. */
    if (world->count >= world->capacity || world->next_entity_id == UINT64_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    for (index = 0U; index < world->capacity; ++index) {
        UmiGameEntity *entity = &world->entities[index];
        /* Inactive slots are fully cleared before reuse to prevent stale state. */
        if (!entity->active) {
            (void)memset(entity, 0, sizeof(*entity));
            written = snprintf(entity->name, sizeof(entity->name), "%s", name);
            /* Truncated names are rejected instead of silently changing identity. */
            if (written < 0 || (size_t)written >= sizeof(entity->name)) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            entity->entity_id = world->next_entity_id;
            entity->kind = kind;
            entity->transform = umi_game_transform_identity();
            entity->active = true;
            *out_entity_id = entity->entity_id;
            world->next_entity_id += 1U;
            world->count += 1U;
            world->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    /* Reaching here means the stored count disagrees with the slot contents. */
    return UMI_STATUS_INTERNAL_ERROR;
}

/* Clear one active slot while preserving monotonic identity allocation. */
UmiStatus umi_game_world_despawn(
    UmiGameWorld *world,
    UmiGameEntityId entity_id)
{
    UmiGameEntity *entity;
    if (world == NULL || entity_id == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    entity = find_mutable(world, entity_id);
    /* A missing identity is distinct from malformed input for editor feedback. */
    if (entity == NULL) return UMI_STATUS_NOT_FOUND;
    (void)memset(entity, 0, sizeof(*entity));
    world->count -= 1U;
    world->revision += 1U;
    return UMI_STATUS_OK;
}

/* Apply a copied transform after rejecting values that collapse an entity. */
UmiStatus umi_game_world_set_transform(
    UmiGameWorld *world,
    UmiGameEntityId entity_id,
    const UmiGameTransform *transform)
{
    UmiGameEntity *entity;
    /* Non-zero scale avoids accidental invisible entities and invalid inverses. */
    if (world == NULL || entity_id == 0U || transform == NULL ||
        transform->scale_x == 0.0 || transform->scale_y == 0.0 ||
        transform->scale_z == 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    entity = find_mutable(world, entity_id);
    if (entity == NULL) return UMI_STATUS_NOT_FOUND;
    entity->transform = *transform;
    world->revision += 1U;
    return UMI_STATUS_OK;
}

/* Replace application-owned tag bits without interpreting their game meaning. */
UmiStatus umi_game_world_set_tags(
    UmiGameWorld *world,
    UmiGameEntityId entity_id,
    uint64_t tags)
{
    UmiGameEntity *entity;
    if (world == NULL || entity_id == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    entity = find_mutable(world, entity_id);
    if (entity == NULL) return UMI_STATUS_NOT_FOUND;
    entity->tags = tags;
    world->revision += 1U;
    return UMI_STATUS_OK;
}

/* Copy an entity by identity so callers cannot mutate or outlive world storage. */
UmiStatus umi_game_world_find(
    const UmiGameWorld *world,
    UmiGameEntityId entity_id,
    UmiGameEntity *out_entity)
{
    size_t index;
    if (world == NULL || entity_id == 0U || out_entity == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < world->capacity; ++index) {
        /* Ignore free slots because their former identity has been invalidated. */
        if (world->entities[index].active &&
            world->entities[index].entity_id == entity_id) {
            *out_entity = world->entities[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/* Enumerate active records while hiding gaps created by despawn operations. */
UmiStatus umi_game_world_at(
    const UmiGameWorld *world,
    size_t index,
    UmiGameEntity *out_entity)
{
    size_t slot;
    size_t active_index = 0U;
    if (world == NULL || out_entity == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= world->count) return UMI_STATUS_NOT_FOUND;
    for (slot = 0U; slot < world->capacity; ++slot) {
        /* Only active records contribute to the public enumeration index. */
        if (world->entities[slot].active) {
            if (active_index == index) {
                *out_entity = world->entities[slot];
                return UMI_STATUS_OK;
            }
            active_index += 1U;
        }
    }
    /* A valid index must resolve unless private count bookkeeping is corrupt. */
    return UMI_STATUS_INTERNAL_ERROR;
}

/* Copy world metadata for renderers, persistence and editor inspectors. */
UmiStatus umi_game_world_snapshot(
    const UmiGameWorld *world,
    UmiGameWorldSnapshot *out_snapshot)
{
    if (world == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Clearing the record prevents uninitialised padding or future fields leaking. */
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = 1U;
    out_snapshot->profile = world->profile;
    out_snapshot->entity_count = world->count;
    out_snapshot->entity_capacity = world->capacity;
    out_snapshot->revision = world->revision;
    return UMI_STATUS_OK;
}
