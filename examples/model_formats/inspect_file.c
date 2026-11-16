/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/model_formats/inspect_file.c
 *
 * PURPOSE:
 *   Inspect and hash one explicitly selected local model file through the installed Framework SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/model_formats/safetensors_file.h"
#include <inttypes.h>
#include <stdio.h>

int main(int argc,char **argv)
{
    if(argc!=2) {
        fprintf(stderr,"Usage: umicom-model-inspect <absolute-model-file>\n");
        return 2;
    }
    UmiSafetensorsFileInspection inspection;
    UmiStatus status=UmiSafetensorsFileInspect(argv[1],NULL,NULL,&inspection);
    if(status!=UMI_STATUS_OK) {
        fprintf(stderr,"Model file inspection refused: %d\n",(int)status);
        return 1;
    }
    char digest[UMI_SHA256_HEX_CAPACITY];
    UmiSha256Hex(inspection.file.sha256,digest);
    printf("Observed bytes: %" PRIu64 "\nSHA-256: %s\nTensors: %zu\nElements: %" PRIu64 "\n",
        inspection.file.bytes,digest,inspection.layout.tensors,inspection.layout.elements);
    puts("File identity and layout observed; publisher trust and model runtime remain unverified.");
    return fflush(stdout)==0 && !ferror(stdout)?0:1;
}
