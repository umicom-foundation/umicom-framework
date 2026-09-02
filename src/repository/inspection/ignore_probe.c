/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/ignore_probe.c
 *
 * PURPOSE:
 *   Implement report whether the repository declares a root .gitignore policy file.
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

#include "umicom/repository/ignore_probe.h"

#include <stdio.h>
/* Test existence only; policy interpretation remains separate from the filesystem probe. */
UmiStatus umi_repository_ignore_probe_read(const UmiRepositoryInspectionContext *context, int *out_present)
{
    char path[4096]; FILE *stream; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (context==NULL||out_present==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (snprintf(path,sizeof(path),"%s/.gitignore",context->repository_root) >= (int)sizeof(path)) return UMI_STATUS_CAPACITY_EXCEEDED;
    stream=fopen(path,"rb"); *out_present=stream!=NULL; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(stream!=NULL) fclose(stream); return UMI_STATUS_OK;
}

