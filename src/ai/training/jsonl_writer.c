/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/jsonl_writer.c
 *
 * PURPOSE:
 *   Emit complete bounded training records through the existing Framework JSON writer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/jsonl_pairs.h"
#include "umicom/protocol/json_writer.h"
#include "umicom/security/secrets.h"
#include <stdlib.h>
#include <string.h>

UmiStatus UmiAiTrainingPairWrite(const UmiAiTrainingPair *pair,
    char *output, size_t capacity, size_t *outBytes)
{
    if (pair == NULL || output == NULL || outBytes == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(pair->sourceId, 0, sizeof(pair->sourceId)) == NULL ||
        memchr(pair->prompt, 0, sizeof(pair->prompt)) == NULL ||
        memchr(pair->completion, 0, sizeof(pair->completion)) == NULL ||
        pair->sourceId[0] == 0 || pair->completion[0] == 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity == 0U) return UMI_STATUS_CAPACITY_EXCEEDED;
    const size_t maximum = UMI_AI_TRAINING_PAIR_LINE_BYTES + 2U;
    size_t temporaryBytes = capacity < maximum ? capacity : maximum;
    char *candidate = malloc(temporaryBytes);
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiJsonWriter writer;
    UmiJsonWriterInit(&writer, candidate, temporaryBytes);
    (void)UmiJsonWriterRaw(&writer, "{\"source_id\":");
    (void)UmiJsonWriterString(&writer, pair->sourceId);
    (void)UmiJsonWriterRaw(&writer, ",\"prompt\":");
    (void)UmiJsonWriterString(&writer, pair->prompt);
    (void)UmiJsonWriterRaw(&writer, ",\"completion\":");
    (void)UmiJsonWriterString(&writer, pair->completion);
    (void)UmiJsonWriterRaw(&writer, "}\n");
    UmiStatus status = writer.status;
    /* The shared writer's errors are sticky. Only a complete serialized
     * record is copied to application memory; the original output survives
     * a late Unicode or capacity error in any field. */
    if (status == UMI_STATUS_OK) {
        memcpy(output, candidate, writer.length + 1U);
        *outBytes = writer.length;
    }
    umi_secret_clear(candidate, temporaryBytes);
    free(candidate);
    return status;
}
