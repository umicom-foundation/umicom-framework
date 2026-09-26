/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/creative_workspace/types.h
 * PURPOSE: Define owned scene, animation and note values shared by creative products.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CREATIVE_WORKSPACE_TYPES_H
#define UMICOM_CREATIVE_WORKSPACE_TYPES_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/media/animation_track.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CREATIVE_ID_CAPACITY 64U
#define UMI_CREATIVE_LABEL_CAPACITY 192U
#define UMI_CREATIVE_MAX_ELEMENTS 48U
#define UMI_CREATIVE_MAX_KEYS 8U
#define UMI_CREATIVE_MAX_NOTES 64U
#define UMI_CREATIVE_HISTORY_CAPACITY 16U
#define UMI_CREATIVE_MAX_EXTENT 1000000
#define UMI_CREATIVE_MAX_DURATION_MS 30000U
#define UMI_CREATIVE_TICKS_PER_BEAT 480U
#define UMI_CREATIVE_SAMPLE_RATE 48000U
#define UMI_CREATIVE_EXPORT_LIMIT (4U * 1024U * 1024U)
    typedef enum UmiCreativeUnit {UMI_CREATIVE_PIXELS = 0, UMI_CREATIVE_MILLIMETRES = 1} UmiCreativeUnit;
    typedef enum UmiCreativeElementKind {
        UMI_CREATIVE_RECTANGLE = 1, UMI_CREATIVE_TEXT = 2, UMI_CREATIVE_PANEL = 3,
        UMI_CREATIVE_CABINET = 4, UMI_CREATIVE_ACTOR = 5, UMI_CREATIVE_OBSTACLE = 6
    } UmiCreativeElementKind;
    /* Offsets are relative to the element's base position. A nonempty animation
 * starts at time zero. Interpolation describes the segment AFTER this key. */
    typedef struct UmiCreativeKey {
        uint32_t timeMs;
        int32_t offsetX, offsetY;
        UmiMediaAnimationInterpolation interpolation;
    } UmiCreativeKey;
    typedef struct UmiCreativeElement {
        char id[UMI_CREATIVE_ID_CAPACITY];
        char label[UMI_CREATIVE_LABEL_CAPACITY];
        UmiCreativeElementKind kind;
        int32_t x, y, width, height;
        uint32_t colourRgb;
        bool visible;
        UmiCreativeKey keys[UMI_CREATIVE_MAX_KEYS];
        size_t keyCount;
    } UmiCreativeElement;
    /* Musical time is integer ticks, not samples. Tempo converts it to playback
 * time. Notes are half-open intervals: an ending note does not overlap a note
 * starting at the same tick. Pitch uses the MIDI note-number convention. */
    typedef struct UmiCreativeNote {
        char id[UMI_CREATIVE_ID_CAPACITY];
        uint32_t startTick, durationTicks;
        uint8_t pitch, velocity;
    } UmiCreativeNote;
    typedef struct UmiCreativeSettings {
        char title[UMI_CREATIVE_LABEL_CAPACITY];
        UmiCreativeUnit unit;
        int32_t width, height;
        uint32_t durationMs, tempoBpm;
    } UmiCreativeSettings;
    /* A caller-owned value, not a live handle. Saving and editing this copy do not
 * mutate a workspace. Replace explicitly validates and commits a complete edit. */
    typedef struct UmiCreativeProject {
        char id[UMI_CREATIVE_ID_CAPACITY];
        UmiCreativeSettings settings;
        UmiCreativeElement elements[UMI_CREATIVE_MAX_ELEMENTS];
        UmiCreativeNote notes[UMI_CREATIVE_MAX_NOTES];
        size_t elementCount, noteCount;
        uint64_t revision;
    } UmiCreativeProject;
    typedef struct UmiCreativeHistory {
        size_t undoCount, redoCount;
        uint64_t revision;
        bool recoveryRequired;
    } UmiCreativeHistory;
    const char * UmiCreativeUnitText(UmiCreativeUnit unit);
    const char * UmiCreativeElementKindText(UmiCreativeElementKind kind);
#ifdef __cplusplus
}

#endif
#endif
