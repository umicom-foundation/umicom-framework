/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/render3d/scene.c
 *
 * PURPOSE:
 *   Implement ownership-safe scene editing independently from a GPU backend,
 *   operating system window or application-specific object model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/render3d/scene.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Private slots pair a public node record with explicit allocation state. */
typedef struct UmiRender3dSceneSlot {
    UmiRender3dNode node;
    bool active;
} UmiRender3dSceneSlot;

/* The scene owns fixed-capacity slots and monotonic node identities. */
struct UmiRender3dScene {
    UmiRender3dSceneSlot *slots;
    size_t capacity;
    size_t count;
    UmiRender3dNodeId next_node_id;
    uint64_t revision;
};

/* Find internal mutable node storage without lending it to a public caller. */
static UmiRender3dNode *find_mutable(
    UmiRender3dScene *scene,
    UmiRender3dNodeId node_id)
{
    size_t index;
    if (scene == NULL || node_id == 0U) return NULL;
    for (index = 0U; index < scene->capacity; ++index) {
        /* Active state prevents a removed identity from resolving through reuse. */
        if (scene->slots[index].active &&
            scene->slots[index].node.node_id == node_id) {
            return &scene->slots[index].node;
        }
    }
    return NULL;
}

/* Copy caller text into a fixed field and reject truncation explicitly. */
static UmiStatus copy_text(char *destination, size_t capacity, const char *text)
{
    int written;
    if (destination == NULL || capacity == 0U || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(destination, capacity, "%s", text);
    /* A truncated resource ID could silently select the wrong asset. */
    return written >= 0 && (size_t)written < capacity
        ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}

/* Allocate scene owner and checked fixed-capacity slot storage. */
UmiStatus umi_render3d_scene_create(
    size_t node_capacity,
    UmiRender3dScene **out_scene)
{
    UmiRender3dScene *scene;
    /* Checked multiplication prevents a wrapped allocation size. */
    if (out_scene == NULL || node_capacity == 0U ||
        node_capacity > SIZE_MAX / sizeof(UmiRender3dSceneSlot)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_scene = NULL;
    scene = (UmiRender3dScene *)calloc(1U, sizeof(*scene));
    if (scene == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    scene->slots = (UmiRender3dSceneSlot *)calloc(
        node_capacity, sizeof(*scene->slots));
    /* A partially allocated scene is destroyed before returning failure. */
    if (scene->slots == NULL) {
        free(scene);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    scene->capacity = node_capacity;
    scene->next_node_id = 1U;
    scene->revision = 1U;
    *out_scene = scene;
    return UMI_STATUS_OK;
}

/* Release slot storage and its owner; NULL destruction is intentionally safe. */
void umi_render3d_scene_destroy(UmiRender3dScene *scene)
{
    if (scene == NULL) return;
    free(scene->slots);
    free(scene);
}

/* Add a node only when its optional parent already belongs to this scene. */
UmiStatus umi_render3d_scene_add_node(
    UmiRender3dScene *scene,
    UmiRender3dNodeId parent_id,
    const char *name,
    const char *mesh_resource_id,
    const char *material_resource_id,
    UmiRender3dNodeId *out_node_id)
{
    size_t index;
    UmiStatus status;
    if (scene == NULL || name == NULL || name[0] == '\0' ||
        mesh_resource_id == NULL || material_resource_id == NULL ||
        out_node_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Non-root nodes require a valid parent, preventing an orphan reference. */
    if (parent_id != 0U && find_mutable(scene, parent_id) == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (scene->count >= scene->capacity || scene->next_node_id == UINT64_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    for (index = 0U; index < scene->capacity; ++index) {
        UmiRender3dSceneSlot *slot = &scene->slots[index];
        /* Free slots are cleared before text fields and transforms are assigned. */
        if (!slot->active) {
            (void)memset(slot, 0, sizeof(*slot));
            status = copy_text(slot->node.name,
                               sizeof(slot->node.name), name);
            if (status == UMI_STATUS_OK) status = copy_text(
                slot->node.mesh_resource_id,
                sizeof(slot->node.mesh_resource_id), mesh_resource_id);
            if (status == UMI_STATUS_OK) status = copy_text(
                slot->node.material_resource_id,
                sizeof(slot->node.material_resource_id), material_resource_id);
            /* No slot becomes active unless every identifying field fits. */
            if (status != UMI_STATUS_OK) return status;
            slot->node.node_id = scene->next_node_id;
            slot->node.parent_id = parent_id;
            slot->node.transform = umi_render3d_transform_identity();
            slot->node.visible = true;
            slot->active = true;
            *out_node_id = slot->node.node_id;
            scene->next_node_id += 1U;
            scene->count += 1U;
            scene->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_INTERNAL_ERROR;
}

/* Remove only a leaf so no remaining node holds a dangling parent identity. */
UmiStatus umi_render3d_scene_remove_node(
    UmiRender3dScene *scene,
    UmiRender3dNodeId node_id)
{
    size_t index;
    UmiRender3dNode *node;
    if (scene == NULL || node_id == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    node = find_mutable(scene, node_id);
    if (node == NULL) return UMI_STATUS_NOT_FOUND;
    for (index = 0U; index < scene->capacity; ++index) {
        /* Parent removal is blocked until children are reparented or removed. */
        if (scene->slots[index].active &&
            scene->slots[index].node.parent_id == node_id) {
            return UMI_STATUS_INVALID_STATE;
        }
    }
    for (index = 0U; index < scene->capacity; ++index) {
        /* Identity comparison locates the owning slot for complete clearing. */
        if (scene->slots[index].active &&
            &scene->slots[index].node == node) {
            (void)memset(&scene->slots[index], 0, sizeof(scene->slots[index]));
            scene->count -= 1U;
            scene->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_INTERNAL_ERROR;
}

/* Update spatial state only when scale and quaternion are mathematically usable. */
UmiStatus umi_render3d_scene_set_transform(
    UmiRender3dScene *scene,
    UmiRender3dNodeId node_id,
    const UmiRender3dTransform *transform)
{
    UmiRender3dNode *node;
    double quaternion_size;
    if (scene == NULL || node_id == 0U || transform == NULL ||
        transform->scale_x == 0.0 || transform->scale_y == 0.0 ||
        transform->scale_z == 0.0) return UMI_STATUS_INVALID_ARGUMENT;
    quaternion_size = transform->rotation_x * transform->rotation_x +
        transform->rotation_y * transform->rotation_y +
        transform->rotation_z * transform->rotation_z +
        transform->rotation_w * transform->rotation_w;
    /* An all-zero quaternion has no valid rotation interpretation. */
    if (quaternion_size == 0.0) return UMI_STATUS_INVALID_ARGUMENT;
    node = find_mutable(scene, node_id);
    if (node == NULL) return UMI_STATUS_NOT_FOUND;
    node->transform = *transform;
    scene->revision += 1U;
    return UMI_STATUS_OK;
}

/* Toggle render inclusion while keeping the node available to editors. */
UmiStatus umi_render3d_scene_set_visible(
    UmiRender3dScene *scene,
    UmiRender3dNodeId node_id,
    bool visible)
{
    UmiRender3dNode *node;
    if (scene == NULL || node_id == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    node = find_mutable(scene, node_id);
    if (node == NULL) return UMI_STATUS_NOT_FOUND;
    node->visible = visible;
    scene->revision += 1U;
    return UMI_STATUS_OK;
}

/* Copy one node by identity so callers cannot retain internal scene pointers. */
UmiStatus umi_render3d_scene_find(
    const UmiRender3dScene *scene,
    UmiRender3dNodeId node_id,
    UmiRender3dNode *out_node)
{
    size_t index;
    if (scene == NULL || node_id == 0U || out_node == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < scene->capacity; ++index) {
        if (scene->slots[index].active &&
            scene->slots[index].node.node_id == node_id) {
            *out_node = scene->slots[index].node;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/* Enumerate active nodes without exposing holes left by removed leaf nodes. */
UmiStatus umi_render3d_scene_at(
    const UmiRender3dScene *scene,
    size_t index,
    UmiRender3dNode *out_node)
{
    size_t slot;
    size_t active_index = 0U;
    if (scene == NULL || out_node == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= scene->count) return UMI_STATUS_NOT_FOUND;
    for (slot = 0U; slot < scene->capacity; ++slot) {
        if (scene->slots[slot].active) {
            /* Public indexes count only active nodes. */
            if (active_index == index) {
                *out_node = scene->slots[slot].node;
                return UMI_STATUS_OK;
            }
            active_index += 1U;
        }
    }
    return UMI_STATUS_INTERNAL_ERROR;
}

/* Count root and visible nodes while copying safe scene metadata. */
UmiStatus umi_render3d_scene_snapshot(
    const UmiRender3dScene *scene,
    UmiRender3dSceneSnapshot *out_snapshot)
{
    size_t index;
    if (scene == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = 1U;
    out_snapshot->node_count = scene->count;
    out_snapshot->node_capacity = scene->capacity;
    out_snapshot->revision = scene->revision;
    for (index = 0U; index < scene->capacity; ++index) {
        /* Only active nodes contribute to public scene statistics. */
        if (scene->slots[index].active) {
            if (scene->slots[index].node.parent_id == 0U) {
                out_snapshot->root_count += 1U;
            }
            if (scene->slots[index].node.visible) {
                out_snapshot->visible_count += 1U;
            }
        }
    }
    return UMI_STATUS_OK;
}
