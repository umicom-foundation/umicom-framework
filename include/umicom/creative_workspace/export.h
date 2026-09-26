/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/creative_workspace/export.h
 * PURPOSE: Create bounded SVG, storyboard, responsive page and PCM-WAVE exports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CREATIVE_WORKSPACE_EXPORT_H
#define UMICOM_CREATIVE_WORKSPACE_EXPORT_H
#include "umicom/creative_workspace/scene.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiCreativeExport { unsigned char * bytes;
        size_t size;
    } UmiCreativeExport;
    typedef enum UmiCreativeExportKind {
        UMI_CREATIVE_EXPORT_SVG = 0, UMI_CREATIVE_EXPORT_HTML = 1,
        UMI_CREATIVE_EXPORT_STORYBOARD = 2, UMI_CREATIVE_EXPORT_WAVE = 3
    } UmiCreativeExportKind;
    /* Initialise outputs to {0}. On success they own allocated bytes; Free releases
 * them. Generation performs no file, network, process or audio-device I/O.
 * HTML is a responsive visual prototype, not generated native application code.
 * Storyboards contain three sampled frames, not encoded video.
 * WAVE is 48000 Hz mono signed 16-bit PCM with a simple sine-wave voice. */
    UmiStatus UmiCreativeExportBuild(const UmiCreativeProject * project, UmiCreativeExportKind kind,
    uint32_t timeMs, UmiCreativeExport * outExport);
    void UmiCreativeExportFree(UmiCreativeExport * exported);
    const char * UmiCreativeExportExtension(UmiCreativeExportKind kind);
    /* Create a new absolute local file only. Existing files are never overwritten.
 * A write failure may leave the newly-created partial file for inspection; this
 * API is not atomic publication. It never removes a path after a failed write. */
    UmiStatus UmiCreativeExportWriteNew(const UmiCreativeExport * exported, const char * absolutePath);
#ifdef __cplusplus
}

#endif
#endif
