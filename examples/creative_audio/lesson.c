/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/creative_audio/lesson.c
 * PURPOSE: Compose notes, render sound, retain a range and release every owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "lesson.h"
#include <stdio.h>
#include <stdlib.h>

UmiStatus UmiCreativeAudioLesson(UmiCreativeExport *optionalOutput)
{
    UmiStatus status;
    UmiCreativeProject *project = calloc(1U, sizeof(*project));
    UmiCreativeExport source = {0}, trimmed = {0};
    UmiCreativeAudioClip *clip = NULL, *result = NULL;
    UmiCreativeAudioInfo info;
    if (optionalOutput != NULL && (optionalOutput->bytes != NULL || optionalOutput->size != 0U)) {
        free(project); return UMI_STATUS_INVALID_STATE;
    }
    if (project == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiCreativeSettings settings = {"Workshop sound", UMI_CREATIVE_PIXELS, 640, 360, 2000U, 120U};
    status = UmiCreativeProjectInit(project, "workshop-sound", &settings);
    const unsigned char pitches[] = {60U, 62U, 64U, 67U};
    for (unsigned i = 0U; status == UMI_STATUS_OK && i < 4U; ++i) {
        UmiCreativeNote note = {0};
        (void)snprintf(note.id, sizeof(note.id), "note-%u", i + 1U);
        note.startTick = i * 480U; note.durationTicks = 480U;
        note.pitch = pitches[i]; note.velocity = 60U;
        status = UmiCreativeProjectPutNote(project, &note, false);
    }
    if (status == UMI_STATUS_OK) status = UmiCreativeExportBuild(project, UMI_CREATIVE_EXPORT_WAVE, 0U, &source);
    if (status == UMI_STATUS_OK) status = UmiCreativeAudioDecode(source.bytes, source.size, &clip);
    /* The imported clip owns a copy. It does not borrow the exported buffer. */
    UmiCreativeExportFree(&source);
    if (status == UMI_STATUS_OK) status = UmiCreativeAudioGetInfo(clip, &info);
    if (status == UMI_STATUS_OK && info.frames != 96000U) status = UMI_STATUS_INTERNAL_ERROR;
    UmiCreativeAudioEdit edit = {12000U, 84000U, 700U, 2400U, 2400U};
    if (status == UMI_STATUS_OK) status = UmiCreativeAudioRender(clip, &edit, &trimmed);
    if (status == UMI_STATUS_OK) status = UmiCreativeAudioDecode(trimmed.bytes, trimmed.size, &result);
    if (status == UMI_STATUS_OK) status = UmiCreativeAudioGetInfo(result, &info);
    if (status == UMI_STATUS_OK && info.frames != 72000U) status = UMI_STATUS_INTERNAL_ERROR;
    if (status == UMI_STATUS_OK) {
        puts("Four notes: 96000 frames at 48000 Hz (2 seconds).");
        puts("Kept frames [12000, 84000): 72000 frames (1.5 seconds).");
        puts("Gain 700/1000; each fade spans 2400 frames. The source remains unchanged.");
        if (optionalOutput != NULL) { *optionalOutput = trimmed; trimmed = (UmiCreativeExport){0}; }
        else puts("Practice complete. Memory only; no file, database or audio device was opened.");
    }
    UmiCreativeAudioDestroy(result); UmiCreativeAudioDestroy(clip);
    UmiCreativeExportFree(&trimmed); free(project);
    return status;
}
