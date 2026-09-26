/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_workspace/export_fixture.c
 * PURPOSE: Generate real exported formats for independent parsers and audio checks.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/creative_workspace/export.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char * * argv)
{
    if (argc!= 3)return 2;
    UmiCreativeProject * p = calloc(1U, sizeof( * p));
    if (p == NULL)return 1;
    UmiCreativeSettings settings = {.title = "Workshop & café <visual>", .unit = UMI_CREATIVE_PIXELS, .width = 960, .height = 600, .durationMs = 8000U, .tempoBpm = 120U};
    UmiStatus status = UmiCreativeProjectInit(p, "fixture", & settings);
    if (status == UMI_STATUS_OK)status = UmiCreativeProjectStarter(p, argv[1]);
    p -> revision = 17U;
    if (strcmp(argv[1], "music") == 0){p -> noteCount = 0U;
        UmiCreativeNote note = {.id = "a4", .startTick = 480U, .durationTicks = 960U, .pitch = 69U, .velocity = 100U};
        if (status == UMI_STATUS_OK)status = UmiCreativeProjectPutNote(p, & note, false);
    }
    const char * names[] = {"drawing.svg", "prototype.html", "storyboard.html", "notes.wav"};
    unsigned count = strcmp(argv[1], "music") == 0 ? 4U : 3U;
    for (unsigned i = 0U;status == UMI_STATUS_OK && i < count; ++ i){
        UmiCreativeExport out = {0};
        char path[4096];
        int n = snprintf(path, sizeof(path), "%s/%s", argv[2], names[i]);
        status = n < 0 || (size_t)n >= sizeof(path) ? UMI_STATUS_CAPACITY_EXCEEDED : UmiCreativeExportBuild(p, (UmiCreativeExportKind)i, 2000U, & out);
        if (status == UMI_STATUS_OK)status = UmiCreativeExportWriteNew( & out, path);
        UmiCreativeExportFree( & out);
    }
    free(p);
    if (status!= UMI_STATUS_OK)fprintf(stderr, "Export fixture status %d\n", (int)status);
    return status == UMI_STATUS_OK ? 0 : 1;
}

