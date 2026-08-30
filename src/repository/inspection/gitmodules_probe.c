/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/gitmodules_probe.c
 *
 * PURPOSE:
 *   Implement bridge on-disk .gitmodules text into the existing canonical submodule inventory parser.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/repository/gitmodules_probe.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/repository/gitmodules.h"
/* Read the configuration file only; parsing remains owned by umi_repository_gitmodules_parse. */
UmiStatus umi_repository_gitmodules_probe_read(const UmiRepositoryInspectionContext *context, UmiRepositoryInventory *out_inventory, int *out_present)
{
    char path[4096]; FILE *stream; char *text; long length; size_t read_count; UmiStatus status;
    if (context == NULL || out_inventory == NULL || out_present == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_present = 0; umi_repository_inventory_init(out_inventory);
    if (snprintf(path, sizeof(path), "%s/.gitmodules", context->repository_root) >= (int)sizeof(path)) return UMI_STATUS_CAPACITY_EXCEEDED;
    stream = fopen(path, "rb"); if (stream == NULL) return UMI_STATUS_OK; *out_present = 1;
    if (fseek(stream, 0, SEEK_END) != 0) { fclose(stream); return UMI_STATUS_IO_ERROR; }
    length = ftell(stream); if (length < 0 || length > 1048576L) { fclose(stream); return UMI_STATUS_CAPACITY_EXCEEDED; }
    rewind(stream); text = (char *)calloc((size_t)length + 1U, 1U); if (text == NULL) { fclose(stream); return UMI_STATUS_OUT_OF_MEMORY; }
    read_count = fread(text, 1U, (size_t)length, stream); fclose(stream); text[read_count] = '\0';
    status = umi_repository_gitmodules_parse(text, out_inventory); free(text); return status;
}

