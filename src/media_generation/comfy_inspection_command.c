/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/comfy_inspection_command.c
 *
 * PURPOSE:
 *   Share offline ComfyUI inspection arguments and output semantics across the native CLI and Media console.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_inspection.h"
#include "umicom/media_generation/comfy_queue_file.h"
#include "umicom/media_generation/comfy_system_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void InspectionUsage(FILE *stream)
{
    fputs("Umicom ComfyUI offline inspection\n"
          "comfy inspect CONNECTION WORKFLOW OBJECT_INFO [--client-id ID]\n"
          "comfy references --help: paged reference-candidate observations\n"
          "comfy queue --help: captured queue observations\n"
          "comfy system --help: captured system and device observations\n"
          "Use absolute paths to a portable connection document, API workflow JSON,\n"
          "and captured /object_info JSON. This command does not contact a provider.\n"
          "Exit 0: required names observed; 3: missing names; 2: usage; 1: failure.\n", stream);
}
int UmiComfyInspectionCommand(int argc, char **argv)
{
    if (argc < 0 || (argc > 0 && argv == NULL)) return 2;
    for (int i = 0; i < argc; ++i) if (argv[i] == NULL) return 2;
    /* System observations remain Framework-owned across CLI, Media and SDK hosts. */
    if (argc > 0 && strcmp(argv[0], "system") == 0)
        return UmiComfySystemStatsFileCommand(argc, argv);
    /* The shared dispatcher gives CLI and Media the same queue file semantics. */
    if (argc > 0 && strcmp(argv[0], "queue") == 0)
        return UmiComfyQueueFileCommand(argc, argv);
    /* Reference observations have their own page/exit contract; keep presence inspection unchanged. */
    if (argc > 0 && strcmp(argv[0], "references") == 0)
        return UmiComfyReferenceInspectionCommand(argc, argv);
    if (argc == 1 && (strcmp(argv[0], "--help") == 0 || strcmp(argv[0], "help") == 0)) {
        InspectionUsage(stdout);
        return ferror(stdout) ? 1 : 0;
    }
    if ((argc != 4 && argc != 6) || strcmp(argv[0], "inspect") != 0 ||
        (argc == 6 && strcmp(argv[4], "--client-id") != 0)) {
        InspectionUsage(stderr);
        return 2;
    }
    const char *client = argc == 6 ? argv[5] : "umicom-offline-inspection";
    size_t length = strlen(client);
    UmiComfyInspectionPaths paths = {0};
    if (length == 0U || length >= sizeof(paths.client_id)) { InspectionUsage(stderr); return 2; }
    memcpy(paths.client_id, client, length + 1U);
    paths.connection_document = argv[1];
    paths.workflow_json = argv[2];
    paths.node_catalogue_json = argv[3];
    UmiComfyPreflightReport *report = NULL;
    UmiComfyPreflightSummary summary;
    char *json = NULL;
    UmiStatus status = UmiComfyInspectionRead(&paths, NULL, &report);
    if (status != UMI_STATUS_OK) goto done;
    status = UmiComfyPreflightInspect(report, &summary);
    if (status != UMI_STATUS_OK) goto done;
    json = malloc(UMI_COMFY_INSPECTION_JSON_CAPACITY);
    if (json == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto done; }
    status = UmiComfyInspectionJson(report, json, UMI_COMFY_INSPECTION_JSON_CAPACITY, &length);
    if (status != UMI_STATUS_OK) goto done;
    /* The explicit origin prevents file metadata from masquerading as a fresh
     * server capability check. Never print workflow values or local filenames. */
    if (fputs("{\"origin\":\"offline-files\",\"report\":", stdout) == EOF ||
        fwrite(json, 1U, length, stdout) != length || fputs("}\n", stdout) == EOF ||
        fflush(stdout) != 0) status = UMI_STATUS_IO_ERROR;
done:
    if (json != NULL) { umi_secret_clear(json, UMI_COMFY_INSPECTION_JSON_CAPACITY); free(json); }
    UmiComfyPreflightDestroy(report);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Umicom workflow inspection: %s\n", umi_status_text(status));
        return 1;
    }
    return summary.all_node_requirements_observed ? 0 : 3;
}
