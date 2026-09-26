/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/export_writer.c
 * PURPOSE: Centralise bounded output ownership, integer formatting and XML escaping.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void UmiCreativeWriterInit(UmiCreativeWriter * w, size_t capacity)
{
    memset(w, 0, sizeof( * w));
    if (capacity == 0U || capacity > UMI_CREATIVE_EXPORT_LIMIT){
        w -> status = UMI_STATUS_CAPACITY_EXCEEDED;
        return;
    }
    w -> bytes = malloc(capacity + 1U);
    if (w -> bytes == NULL){
        w -> status = UMI_STATUS_OUT_OF_MEMORY;
        return;
    }
    w -> capacity = capacity;
    w -> bytes[0] = 0U;
    w -> status = UMI_STATUS_OK;
}

void UmiCreativeWriterAppend(UmiCreativeWriter * w, const void * bytes, size_t size)
{
    if (w -> status!= UMI_STATUS_OK)return;
    if (size > w -> capacity - w -> size){
        w -> status = UMI_STATUS_CAPACITY_EXCEEDED;
        return;
    }
    if (size > 0U)memcpy(w -> bytes + w -> size, bytes, size);
    w -> size += size;
    w -> bytes[w -> size] = 0U;
}

void UmiCreativeWriterText(UmiCreativeWriter * w, const char * text){
    UmiCreativeWriterAppend(w, text, strlen(text));
}

void UmiCreativeWriterNumber(UmiCreativeWriter * w, uint64_t value)
{
    char text[32];
    (void)snprintf(text, sizeof(text), "%" PRIu64, value);
    UmiCreativeWriterText(w, text);
}

void UmiCreativeWriterXml(UmiCreativeWriter * w, const char * text)
{
    for (size_t i = 0U;text[i]!= '\0'; ++ i){
        switch (text[i]){
            case '&':UmiCreativeWriterText(w, "&amp;");
            break;
            case '<':UmiCreativeWriterText(w, "&lt;");
            break;
            case '>':UmiCreativeWriterText(w, "&gt;");
            break;
            case '"':UmiCreativeWriterText(w, "&quot;");
            break;
            case '\'':UmiCreativeWriterText(w, "&#39;");
            break;
            default : UmiCreativeWriterAppend(w, & text[i], 1U);
            break;
        }
    }
}

UmiStatus UmiCreativeWriterFinish(UmiCreativeWriter * w, UmiCreativeExport * out)
{
    UmiStatus status = w -> status;
    if (status == UMI_STATUS_OK){
        out -> bytes = w -> bytes;
        out -> size = w -> size;
    }
    else{
        free(w -> bytes);
        out -> bytes = NULL;
        out -> size = 0U;
    }
    w -> bytes = NULL;
    w -> size = 0U;
    w -> capacity = 0U;
    return status;
}

void UmiCreativeExportFree(UmiCreativeExport * exported)
{
    if (exported!= NULL){
        free(exported -> bytes);
        exported -> bytes = NULL;
        exported -> size = 0U;
    }
}

const char * UmiCreativeExportExtension(UmiCreativeExportKind kind)
{
    switch (kind){
        case UMI_CREATIVE_EXPORT_SVG:return ".svg";
        case UMI_CREATIVE_EXPORT_HTML:case UMI_CREATIVE_EXPORT_STORYBOARD:return ".html";
        case UMI_CREATIVE_EXPORT_WAVE:return ".wav";
        default : return NULL;
    }
}

UmiStatus UmiCreativeExportBuild(const UmiCreativeProject * p, UmiCreativeExportKind kind, uint32_t timeMs, UmiCreativeExport * out)
{
    if (out == NULL)return UMI_STATUS_INVALID_ARGUMENT;
    /* Reusing a live output would leak its allocation. Require explicit Free. */
    if (out -> bytes!= NULL || out -> size!= 0U)return UMI_STATUS_INVALID_STATE;
    if (UmiCreativeProjectValidate(p)!= UMI_STATUS_OK || timeMs > p -> settings.durationMs)return UMI_STATUS_INVALID_ARGUMENT;
    switch (kind){
        case UMI_CREATIVE_EXPORT_SVG:return UmiCreativeExportSvg(p, timeMs, out);
        case UMI_CREATIVE_EXPORT_HTML:return UmiCreativeExportHtml(p, timeMs, false, out);
        case UMI_CREATIVE_EXPORT_STORYBOARD:return UmiCreativeExportHtml(p, timeMs, true, out);
        case UMI_CREATIVE_EXPORT_WAVE:return UmiCreativeExportWave(p, out);
        default : return UMI_STATUS_INVALID_ARGUMENT;
    }
}

