/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/export_writer_json.c
 *
 * PURPOSE:
 *   Connect full creative documents to the shared JSON string encoder.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include "umicom/ai/mcp/json.h"
#include <string.h>

/* Keep the same bounded text ownership and quoting contract. Full creative
 * documents link the established encoder while the audio writer stays portable. */
void UmiCreativeWriterJson(UmiCreativeWriter *writer, const char *text)
{
    if (writer->status != UMI_STATUS_OK) return;
    writer->status = umi_ai_mcp_json_escape_string(text,
        (char *)writer->bytes + writer->size, writer->capacity - writer->size + 1U);
    if (writer->status == UMI_STATUS_OK)
        writer->size += strlen((const char *)writer->bytes + writer->size);
}
