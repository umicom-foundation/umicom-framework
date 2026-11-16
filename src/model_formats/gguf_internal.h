/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/model_formats/gguf_internal.h
 *
 * PURPOSE:
 *   Share a fixed-size incremental GGUF scanner between prefix and checked file inspection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_MODEL_FORMATS_GGUF_INTERNAL_H
#define UMICOM_MODEL_FORMATS_GGUF_INTERNAL_H
#include "umicom/model_formats/gguf.h"
typedef enum GgufPhase {
    GGUF_HEADER,GGUF_KEY_LENGTH,GGUF_KEY,GGUF_VALUE_TYPE,GGUF_SCALAR,
    GGUF_STRING_LENGTH,GGUF_STRING,GGUF_ARRAY_TYPE,GGUF_ARRAY_LENGTH,GGUF_DONE
} GgufPhase;
typedef struct GgufArray { uint32_t type; uint64_t remaining; } GgufArray;
typedef struct GgufScan {
    UmiGgufMetadata metadata;
    const UmiCancellationToken *cancel;
    GgufPhase phase;
    uint64_t remaining,consumed,entries_done,values;
    size_t used,depth;
    unsigned char field[24];
    char key[512];
    unsigned selected;
    uint32_t type,array_type;
    GgufArray arrays[UMI_GGUF_MAX_ARRAY_DEPTH];
} GgufScan;
void GgufScanInit(GgufScan *scan,const UmiCancellationToken *cancel);
UmiStatus GgufScanFeed(GgufScan *scan,const unsigned char *bytes,size_t size);
UmiStatus GgufScanFinish(GgufScan *scan,UmiGgufMetadata *out);
#endif
