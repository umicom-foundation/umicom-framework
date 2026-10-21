/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/corpus.c
 * PURPOSE: Native read-only training corpus and bounded next-token windows.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/corpus.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiAiCorpus {
    uint8_t *bytes;
    size_t length;
};

UmiStatus UmiAiCorpusCreate(const uint8_t *bytes, size_t length,
                            UmiAiCorpus **outCorpus)
{
    UmiAiCorpus *created;
    if (outCorpus == NULL || bytes == NULL || length == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (length > UMI_AI_CORPUS_MAX_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    created = calloc(1U, sizeof(*created));
    if (created == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    created->bytes = malloc(length);
    if (created->bytes == NULL) { free(created); return UMI_STATUS_OUT_OF_MEMORY; }
    memcpy(created->bytes, bytes, length);
    created->length = length;
    *outCorpus = created;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiCorpusLoadFile(const char *path, UmiAiCorpus **outCorpus)
{
    FILE *file;
    uint8_t *buffer;
    long measured;
    size_t size;
    UmiStatus status;
    if (path == NULL || path[0] == '\0' || outCorpus == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    file = fopen(path, "rb");
    if (file == NULL) return UMI_STATUS_IO_ERROR;
    /* Guard fseek/ftell failures and do not allocate from untrusted sizes. */
    if (fseek(file, 0L, SEEK_END) != 0) { fclose(file); return UMI_STATUS_IO_ERROR; }
    measured = ftell(file);
    if (measured < 0L) { fclose(file); return UMI_STATUS_IO_ERROR; }
    if ((unsigned long)measured > UMI_AI_CORPUS_MAX_BYTES) {
        fclose(file);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (measured == 0L) { fclose(file); return UMI_STATUS_INVALID_ARGUMENT; }
    if (fseek(file, 0L, SEEK_SET) != 0) { fclose(file); return UMI_STATUS_IO_ERROR; }
    size = (size_t)measured;
    buffer = malloc(size);
    if (buffer == NULL) { fclose(file); return UMI_STATUS_OUT_OF_MEMORY; }
    if (fread(buffer, 1U, size, file) != size) {
        free(buffer); fclose(file); return UMI_STATUS_IO_ERROR;
    }
    /* Detect an appended byte, so changing files cannot produce a silently
     * incomplete training corpus while the file is being imported. */
    if (fgetc(file) != EOF || ferror(file) != 0) {
        free(buffer); fclose(file); return UMI_STATUS_IO_ERROR;
    }
    if (fclose(file) != 0) { free(buffer); return UMI_STATUS_IO_ERROR; }
    status = UmiAiCorpusCreate(buffer, size, outCorpus);
    free(buffer);
    return status;
}

void UmiAiCorpusDestroy(UmiAiCorpus *corpus)
{
    if (corpus == NULL) return;
    free(corpus->bytes);
    free(corpus);
}

UmiStatus UmiAiCorpusLength(const UmiAiCorpus *corpus, size_t *outLength)
{
    if (corpus == NULL || outLength == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outLength = corpus->length;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiCorpusWindow(const UmiAiCorpus *corpus,
                            size_t start, size_t count,
                            uint32_t *inputs, uint32_t *targets,
                            size_t capacity)
{
    if (corpus == NULL || inputs == NULL || targets == NULL ||
        count == 0U || inputs == targets || start >= corpus->length ||
        count > corpus->length - start - 1U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (count > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < count; ++i) {
        inputs[i] = (uint32_t)corpus->bytes[start + i] + UMI_AI_BYTE_OFFSET;
        targets[i] = (uint32_t)corpus->bytes[start + i + 1U] + UMI_AI_BYTE_OFFSET;
    }
    return UMI_STATUS_OK;
}
