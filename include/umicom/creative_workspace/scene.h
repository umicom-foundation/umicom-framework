/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/creative_workspace/scene.h
 * PURPOSE: Project a scene through the shared animation engine and test bounded motion.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CREATIVE_WORKSPACE_SCENE_H
#define UMICOM_CREATIVE_WORKSPACE_SCENE_H
#include "umicom/creative_workspace/project.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiCreativeFrameElement {
        char id[UMI_CREATIVE_ID_CAPACITY], label[UMI_CREATIVE_LABEL_CAPACITY];
        double x, y, width, height;
        UmiCreativeElementKind kind;
        uint32_t colourRgb;
        bool visible;
    } UmiCreativeFrameElement;
    typedef struct UmiCreativeFrame {
        UmiCreativeFrameElement elements[UMI_CREATIVE_MAX_ELEMENTS];
        size_t elementCount;
        uint32_t timeMs;
        uint64_t sourceRevision;
    } UmiCreativeFrame;
    UmiStatus UmiCreativeSceneSample(const UmiCreativeProject * project, uint32_t timeMs, UmiCreativeFrame * outFrame);
    /* Reverse paint order chooses the topmost visible element; bounds are half-open. */
    UmiStatus UmiCreativeSceneHitTest(const UmiCreativeFrame * frame, double x, double y, size_t * outIndex);
    typedef struct UmiCreativeOverlap { size_t first, second;
    } UmiCreativeOverlap;
    /* Text boxes are excluded. Touching edges are not intersections. capacity=0
 * may be used to count pairs; insufficient capacity returns CAPACITY_EXCEEDED. */
    UmiStatus UmiCreativeSceneOverlaps(const UmiCreativeFrame * frame, UmiCreativeOverlap * pairs,
    size_t capacity, size_t * outCount);
    /* A transient play position, not a saved edit. This small collision playground
 * accepts a single axis at a time and static scenes only. It sweeps the entire
 * requested distance, so a large step cannot jump through a thin obstacle. */
    typedef struct UmiCreativePlayState { char actorId[UMI_CREATIVE_ID_CAPACITY];
        int32_t x, y;
        uint64_t sourceRevision;
    } UmiCreativePlayState;
    typedef struct UmiCreativeMotion { int32_t movedX, movedY;
        bool blocked;
        char blockerId[UMI_CREATIVE_ID_CAPACITY];
    } UmiCreativeMotion;
    UmiStatus UmiCreativePlayBegin(const UmiCreativeProject * project, const char * actorId, UmiCreativePlayState * outState);
    UmiStatus UmiCreativePlayMove(const UmiCreativeProject * project, UmiCreativePlayState * state,
    int32_t deltaX, int32_t deltaY, UmiCreativeMotion * outMotion);
#ifdef __cplusplus
}

#endif
#endif
