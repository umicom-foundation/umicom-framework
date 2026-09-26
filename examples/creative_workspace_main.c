/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/creative_workspace_main.c
 * PURPOSE: Run a real in-memory creative workflow and optionally write an explicitly requested export.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/creative_workspace/workspace.h"
#include "umicom/creative_workspace/export.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef UMICOM_CREATIVE_PROFILE
#define UMICOM_CREATIVE_PROFILE "media"
#endif
int main(int argc, char * * argv)
{
    UmiCreativeExportKind kind = strcmp(UMICOM_CREATIVE_PROFILE, "music") == 0 ? UMI_CREATIVE_EXPORT_WAVE : UMI_CREATIVE_EXPORT_SVG;
    if (argc!= 1 && argc!= 4){fprintf(stderr, "Usage: %s [--export svg|html|storyboard|wav ABSOLUTE_NEW_FILE]\n", argv[0]);
        return 2;
    }
    if (argc == 4){
        if (strcmp(argv[1], "--export")!= 0)return 2;
        if (strcmp(argv[2], "svg") == 0)kind = UMI_CREATIVE_EXPORT_SVG;
        else if (strcmp(argv[2], "html") == 0)kind = UMI_CREATIVE_EXPORT_HTML;
        else if (strcmp(argv[2], "storyboard") == 0)kind = UMI_CREATIVE_EXPORT_STORYBOARD;
        else if (strcmp(argv[2], "wav") == 0)kind = UMI_CREATIVE_EXPORT_WAVE;
        else return 2;
    }
    UmiCreativeProject * project = calloc(1U, sizeof( * project));
    if (project == NULL)return 1;
    UmiCreativeSettings settings = {.title = "Umicom workshop design", .unit = UMI_CREATIVE_PIXELS, .width = 960, .height = 600, .durationMs = 8000U, .tempoBpm = 120U};
    UmiDataServer * server = NULL;
    UmiCreativeWorkspace * workspace = NULL;
    UmiCreativeExport exported = {0};
    UmiStatus status = UmiCreativeProjectInit(project, "workshop-example", & settings);
    if (status == UMI_STATUS_OK)status = umi_data_server_create_memory( & server);
    if (status == UMI_STATUS_OK)status = UmiCreativeWorkspaceCreate(server, project, & workspace);
    if (status == UMI_STATUS_OK)status = UmiCreativeWorkspaceCopy(workspace, project);
    if (status == UMI_STATUS_OK)status = UmiCreativeProjectStarter(project, UMICOM_CREATIVE_PROFILE);
    if (status == UMI_STATUS_OK)status = UmiCreativeWorkspaceReplace(workspace, project, project -> revision);
    if (status == UMI_STATUS_OK)status = UmiCreativeWorkspaceCopy(workspace, project);
    if (status == UMI_STATUS_OK)status = UmiCreativeExportBuild(project, kind, 2000U, & exported);
    if (status == UMI_STATUS_OK){
        printf("Profile: %s\nProject: %s, revision %" PRIu64 "\nObjects: %zu; notes: %zu\nExport generated: %zu bytes\n",
        UMICOM_CREATIVE_PROFILE, project -> id, project -> revision, project -> elementCount, project -> noteCount, exported.size);
        if (argc == 4){status = UmiCreativeExportWriteNew( & exported, argv[3]);
            if (status == UMI_STATUS_OK)printf("New export written: %s\n", argv[3]);
        }
        else puts("Practice complete. The export stayed in memory. No file, network or audio-device operation was requested.");
    }
    if (status!= UMI_STATUS_OK)fprintf(stderr, "Creative workflow failed with status %d.\n", (int)status);
    UmiCreativeExportFree( & exported);
    UmiCreativeWorkspaceDestroy(workspace);
    umi_data_server_destroy(server);
    free(project);
    return status == UMI_STATUS_OK ? 0 : 1;
}

