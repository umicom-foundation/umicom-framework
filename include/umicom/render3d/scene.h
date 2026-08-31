/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/render3d/scene.h
 *
 * PURPOSE:
 *   Own a bounded toolkit-neutral scene graph and protect parent-child
 *   relationships from dangling references during editing and rendering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RENDER3D_SCENE_H
#define UMICOM_RENDER3D_SCENE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/render3d/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The opaque scene owns every node record and never exports mutable pointers. */
typedef struct UmiRender3dScene UmiRender3dScene;

/* A scene snapshot supports inspectors and render scheduling without ownership. */
typedef struct UmiRender3dSceneSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t node_count;
    size_t node_capacity;
    size_t root_count;
    size_t visible_count;
    uint64_t revision;
} UmiRender3dSceneSnapshot;

/* Create a scene with explicit bounded node capacity. */
UmiStatus umi_render3d_scene_create(
    size_t node_capacity,
    UmiRender3dScene **out_scene);

/* Destroy the scene and all node records it owns. */
void umi_render3d_scene_destroy(UmiRender3dScene *scene);

/* Add a root or child node with portable mesh and material resource IDs. */
UmiStatus umi_render3d_scene_add_node(
    UmiRender3dScene *scene,
    UmiRender3dNodeId parent_id,
    const char *name,
    const char *mesh_resource_id,
    const char *material_resource_id,
    UmiRender3dNodeId *out_node_id);

/* Remove a leaf node; parents with children are protected from dangling links. */
UmiStatus umi_render3d_scene_remove_node(
    UmiRender3dScene *scene,
    UmiRender3dNodeId node_id);

/* Replace one node transform after validating scale and quaternion presence. */
UmiStatus umi_render3d_scene_set_transform(
    UmiRender3dScene *scene,
    UmiRender3dNodeId node_id,
    const UmiRender3dTransform *transform);

/* Change whether a renderer should include the node in a render plan. */
UmiStatus umi_render3d_scene_set_visible(
    UmiRender3dScene *scene,
    UmiRender3dNodeId node_id,
    bool visible);

/* Copy a node by stable identity into caller-owned storage. */
UmiStatus umi_render3d_scene_find(
    const UmiRender3dScene *scene,
    UmiRender3dNodeId node_id,
    UmiRender3dNode *out_node);

/* Copy the node at one active enumeration position. */
UmiStatus umi_render3d_scene_at(
    const UmiRender3dScene *scene,
    size_t index,
    UmiRender3dNode *out_node);

/* Copy scene counts and revision for safe rendering and editor refresh. */
UmiStatus umi_render3d_scene_snapshot(
    const UmiRender3dScene *scene,
    UmiRender3dSceneSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
