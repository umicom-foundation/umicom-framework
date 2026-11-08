/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/file_observation.c
 *
 * PURPOSE:
 *   Compose bounded native file reads and shared SHA-256 into owned observations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/toolchain/file_observation.h"
#include "umicom/base/sha256.h"
#include "umicom/base/utf8.h"
#include "umicom/platform/input_file.h"
#include "umicom/platform/output_file.h"
#include "umicom/platform/path.h"
#include "umicom/protocol/json_writer.h"
#include <stdlib.h>
#include <string.h>

struct UmiFileObservation {
    size_t count, totalBytes;
    UmiFileObservationItem items[UMI_FILE_OBSERVATION_MAX_FILES];
    char paths[UMI_FILE_OBSERVATION_MAX_FILES][UMI_PATH_CAPACITY];
};
UmiStatus UmiFileObservationCapture(const char *const *paths, size_t count,
    size_t maximumFileBytes, UmiFileObservation **outObservation, size_t *outFailedIndex)
{
    if (outFailedIndex != NULL) *outFailedIndex = SIZE_MAX;
    if (paths == NULL || count == 0U || count > UMI_FILE_OBSERVATION_MAX_FILES ||
        maximumFileBytes == 0U || maximumFileBytes > UMI_FILE_OBSERVATION_MAX_BYTES ||
        outObservation == NULL || *outObservation != NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < count; ++i) {
        UmiStatus status = UmiOutputFileValidatePath(paths[i]);
        if (status == UMI_STATUS_OK && !UmiUtf8Validate((const unsigned char *)paths[i], strlen(paths[i]), NULL))
            status = UMI_STATUS_INVALID_ARGUMENT;
        for (size_t j = 0U; status == UMI_STATUS_OK && j < i; ++j)
            if (strcmp(paths[i], paths[j]) == 0) status = UMI_STATUS_INVALID_ARGUMENT;
        if (status != UMI_STATUS_OK) {
            if (outFailedIndex != NULL) *outFailedIndex = i;
            return status;
        }
    }
    UmiFileObservation *observation = calloc(1U, sizeof *observation);
    if (observation == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    observation->count = count;
    /* Copy the entire selection before I/O. Results never borrow CLI argument or
     * widget storage, and formatting cannot reread a subsequently changed path. */
    for (size_t i = 0U; i < count; ++i) {
        memcpy(observation->paths[i], paths[i], strlen(paths[i]) + 1U);
        observation->items[i].path = observation->paths[i];
    }
    for (size_t i = 0U; i < count; ++i) {
        size_t allowance = UMI_FILE_OBSERVATION_TOTAL_BYTES - observation->totalBytes;
        if (allowance > maximumFileBytes) allowance = maximumFileBytes;
        unsigned char *bytes = NULL;
        size_t length = 0U;
        UmiStatus status = UmiInputFileRead(observation->paths[i], allowance, &bytes, &length);
        if (status == UMI_STATUS_OK)
            status = UmiSha256Buffer(bytes, length, observation->items[i].sha256);
        UmiInputFileFree(bytes);
        if (status != UMI_STATUS_OK) {
            if (outFailedIndex != NULL) *outFailedIndex = i;
            free(observation);
            return status;
        }
        observation->items[i].bytes = length;
        observation->totalBytes += length;
    }
    *outObservation = observation;
    return UMI_STATUS_OK;
}
void UmiFileObservationDestroy(UmiFileObservation *observation) { free(observation); }
size_t UmiFileObservationCount(const UmiFileObservation *observation)
{ return observation != NULL ? observation->count : 0U; }
const UmiFileObservationItem *UmiFileObservationAt(const UmiFileObservation *observation, size_t index)
{ return observation != NULL && index < observation->count ? &observation->items[index] : NULL; }
UmiStatus UmiFileObservationFormat(const UmiFileObservation *observation,
    char *json, size_t capacity, size_t *outSize)
{
    if (outSize != NULL) *outSize = 0U;
    if (json == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    json[0] = '\0';
    if (observation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiJsonWriter writer;
    UmiJsonWriterInit(&writer, json, capacity);
    (void)UmiJsonWriterRaw(&writer, "{\"schema\":\"umicom.file-observation\",\"version\":1,"
        "\"algorithm\":\"sha256\",\"atomicSnapshot\":false,\"loadedModuleIdentity\":false,"
        "\"qualificationPassed\":false,\"totalBytes\":");
    (void)UmiJsonWriterUInt64(&writer, observation->totalBytes);
    (void)UmiJsonWriterRaw(&writer, ",\"files\":[");
    for (size_t i = 0U; i < observation->count; ++i) {
        const UmiFileObservationItem *item = &observation->items[i];
        if (i != 0U) (void)UmiJsonWriterRaw(&writer, ",");
        (void)UmiJsonWriterRaw(&writer, "{\"path\":");
        (void)UmiJsonWriterString(&writer, item->path);
        (void)UmiJsonWriterRaw(&writer, ",\"bytes\":");
        (void)UmiJsonWriterUInt64(&writer, item->bytes);
        (void)UmiJsonWriterRaw(&writer, ",\"sha256\":");
        (void)UmiJsonWriterString(&writer, item->sha256);
        (void)UmiJsonWriterRaw(&writer, "}");
    }
    (void)UmiJsonWriterRaw(&writer, "]}\n");
    if (writer.status != UMI_STATUS_OK) json[0] = '\0';
    else if (outSize != NULL) *outSize = writer.length;
    return writer.status;
}
UmiStatus UmiFileObservationSave(const UmiFileObservation *observation, const char *path)
{
    UmiStatus status = UmiOutputFileValidatePath(path);
    if (status != UMI_STATUS_OK) return status;
    char *json = malloc(UMI_FILE_OBSERVATION_JSON_CAPACITY);
    if (json == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    size_t length = 0U;
    status = UmiFileObservationFormat(observation, json, UMI_FILE_OBSERVATION_JSON_CAPACITY, &length);
    UmiOutputFile *file = NULL;
    if (status == UMI_STATUS_OK) status = UmiOutputFileCreate(path, &file);
    if (status == UMI_STATUS_OK) {
        status = UmiOutputFileWrite(file, json, length);
        UmiStatus closed = UmiOutputFileClose(file);
        if (status == UMI_STATUS_OK) status = closed;
    }
    UmiOutputFileDestroy(file);
    free(json);
    return status;
}
