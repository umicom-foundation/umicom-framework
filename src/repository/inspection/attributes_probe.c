/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/attributes_probe.c
 *
 * PURPOSE:
 *   Implement report whether the repository declares a root .gitattributes policy file.
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

#include "umicom/repository/attributes_probe.h"

#include <stdio.h>
/* Test existence only so attribute validation can be applied by policy modules. */
UmiStatus umi_repository_attributes_probe_read(const UmiRepositoryInspectionContext *context, int *out_present)
{
    char path[4096]; FILE *stream; if (context==NULL||out_present==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (snprintf(path,sizeof(path),"%s/.gitattributes",context->repository_root) >= (int)sizeof(path)) return UMI_STATUS_CAPACITY_EXCEEDED;
    stream=fopen(path,"rb"); *out_present=stream!=NULL; if(stream!=NULL) fclose(stream); return UMI_STATUS_OK;
}

