/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/render3d/test_scene.c
 *
 * PURPOSE:
 *   Verify scene ownership, parent protection and copied node snapshots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/render3d/render3d.h"

/* Exercise a small parent-child scene without requiring a graphics adapter. */
int main(void)
{
    UmiRender3dScene *scene = NULL;
    UmiRender3dNodeId room_id = 0U;
    UmiRender3dNodeId cabinet_id = 0U;
    UmiRender3dNode cabinet;
    UmiRender3dSceneSnapshot snapshot;
    UmiRender3dTransform transform = umi_render3d_transform_identity();

    /* Child creation verifies portable resource IDs and valid parent ownership. */
    assert(umi_render3d_scene_create(4U, &scene) == UMI_STATUS_OK);
    assert(umi_render3d_scene_add_node(
        scene, 0U, "Room", "mesh.room", "material.wall", &room_id) ==
        UMI_STATUS_OK);
    assert(umi_render3d_scene_add_node(
        scene, room_id, "Cabinet", "mesh.cabinet", "material.wood",
        &cabinet_id) == UMI_STATUS_OK);
    transform.translation_x = 2.0;
    assert(umi_render3d_scene_set_transform(
        scene, cabinet_id, &transform) == UMI_STATUS_OK);
    assert(umi_render3d_scene_find(scene, cabinet_id, &cabinet) ==
           UMI_STATUS_OK);
    assert(strcmp(cabinet.name, "Cabinet") == 0);
    assert(cabinet.transform.translation_x == 2.0);

    /* A parent cannot be removed while a child still references its identity. */
    assert(umi_render3d_scene_remove_node(scene, room_id) ==
           UMI_STATUS_INVALID_STATE);
    assert(umi_render3d_scene_snapshot(scene, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.node_count == 2U);
    assert(snapshot.root_count == 1U);
    assert(snapshot.visible_count == 2U);

    /* Leaf-first removal preserves relationship integrity. */
    assert(umi_render3d_scene_remove_node(scene, cabinet_id) == UMI_STATUS_OK);
    assert(umi_render3d_scene_remove_node(scene, room_id) == UMI_STATUS_OK);
    umi_render3d_scene_destroy(scene);
    return 0;
}
