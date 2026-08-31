/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/render3d/types.h
 *
 * PURPOSE:
 *   Define serialisable 3D transforms and scene-node records for CAD, kitchen
 *   design, object modelling, game and visualisation applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RENDER3D_TYPES_H
#define UMICOM_RENDER3D_TYPES_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_RENDER3D_NAME_CAPACITY 64U
#define UMI_RENDER3D_RESOURCE_ID_CAPACITY 96U

/* Scene identities are numeric so they remain stable across storage changes. */
typedef uint64_t UmiRender3dNodeId;

/* A transform uses translation, quaternion rotation and independent scale. */
typedef struct UmiRender3dTransform {
    double translation_x;
    double translation_y;
    double translation_z;
    double rotation_x;
    double rotation_y;
    double rotation_z;
    double rotation_w;
    double scale_x;
    double scale_y;
    double scale_z;
} UmiRender3dTransform;

/* A node references portable resource IDs instead of GPU or toolkit handles. */
typedef struct UmiRender3dNode {
    UmiRender3dNodeId node_id;
    UmiRender3dNodeId parent_id;
    char name[UMI_RENDER3D_NAME_CAPACITY];
    char mesh_resource_id[UMI_RENDER3D_RESOURCE_ID_CAPACITY];
    char material_resource_id[UMI_RENDER3D_RESOURCE_ID_CAPACITY];
    UmiRender3dTransform transform;
    bool visible;
} UmiRender3dNode;

/* Create a neutral transform with identity rotation and unit scale. */
UmiRender3dTransform umi_render3d_transform_identity(void);

#ifdef __cplusplus
}
#endif

#endif
