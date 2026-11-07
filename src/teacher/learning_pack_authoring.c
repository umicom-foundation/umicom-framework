/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/learning_pack_authoring.c
 *
 * PURPOSE:
 *   Reuse bounded JSON, rooted-file reads and SHA-256 for explicit learning-pack revisions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "learning_pack_internal.h"
#include "umicom/teacher/learning_pack_authoring.h"
#include "umicom/language_runtime/json_writer.h"
#include "umicom/platform/rooted_files.h"
#include <stdlib.h>

typedef UmiLanguageRuntimeJsonWriter PackWriter;
static void Raw(PackWriter *writer, const char *text)
{
    (void)umi_language_runtime_json_writer_raw(writer, text);
}
static void Text(PackWriter *writer, const char *text)
{
    (void)umi_language_runtime_json_writer_string(writer, text);
}
static void Number(PackWriter *writer, uint64_t number)
{
    (void)umi_language_runtime_json_writer_uint64(writer, number);
}
static void Version(PackWriter *writer, UmiVersion version)
{
    Raw(writer, "["); Number(writer, version.major);
    Raw(writer, ","); Number(writer, version.minor);
    Raw(writer, ","); Number(writer, version.patch); Raw(writer, "]");
}
static const char *Kind(UmiLearningResourceKind kind)
{
    switch (kind) {
    case UMI_LEARNING_RESOURCE_LESSON: return "lesson";
    case UMI_LEARNING_RESOURCE_GUIDE: return "guide";
    case UMI_LEARNING_RESOURCE_EXAMPLE: return "example";
    case UMI_LEARNING_RESOURCE_IMAGE: return "image";
    default: return NULL;
    }
}

UmiStatus UmiLearningPackPrepareRevision(const UmiLearningPack *source,
    uint64_t new_revision, size_t maximum_resource_bytes,
    unsigned char **out_json, size_t *out_length)
{
    if (out_json != NULL) *out_json = NULL;
    if (out_length != NULL) *out_length = 0U;
    if (source == NULL || out_json == NULL || out_length == NULL ||
        maximum_resource_bytes == 0U ||
        maximum_resource_bytes > UMI_LEARNING_AUTHORING_RESOURCE_BYTES)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (new_revision <= source->info.revision || new_revision > (uint64_t)INT64_MAX)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Stage the entire manifest before exposing it. Reuse the shared writer
     * so quotes, backslashes and Unicode titles follow the reader's schema. */
    char *json = malloc((size_t)UMI_LEARNING_PACK_MANIFEST_BYTES + 1U);
    if (json == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    PackWriter writer;
    umi_language_runtime_json_writer_init(&writer, json,
        (size_t)UMI_LEARNING_PACK_MANIFEST_BYTES + 1U);
    const UmiLearningPackInfo *info = &source->info;
    Raw(&writer, "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":");
    Text(&writer, info->id); Raw(&writer, ",\"title\":"); Text(&writer, info->title);
    Raw(&writer, ",\"product\":"); Text(&writer, info->product);
    Raw(&writer, ",\"locale\":"); Text(&writer, info->locale);
    Raw(&writer, ",\"revision\":"); Number(&writer, new_revision);
    Raw(&writer, ",\"compatibility\":{\"minimum\":"); Version(&writer, info->minimum_version);
    Raw(&writer, ",\"maximum\":"); Version(&writer, info->maximum_version);
    Raw(&writer, "},\"resources\":[");
    UmiStatus status = writer.status;
    for (size_t i = 0U; i < info->resource_count && status == UMI_STATUS_OK; ++i) {
        const UmiLearningPackResource *item = &source->resources[i];
        const char *kind = Kind(item->kind);
        if (kind == NULL) { status = UMI_STATUS_INVALID_STATE; break; }
        unsigned char *bytes = NULL;
        size_t length = 0U;
        char digest[UMI_SHA256_HEX_CAPACITY];
        status = UmiRootedFileRead(source->root, item->path,
            maximum_resource_bytes, &bytes, &length);
        if (status == UMI_STATUS_OK) status = UmiSha256Buffer(bytes, length, digest);
        UmiRootedFileFree(bytes);
        if (status != UMI_STATUS_OK) break;
        if (i != 0U) Raw(&writer, ",");
        Raw(&writer, "{\"id\":"); Text(&writer, item->id);
        Raw(&writer, ",\"title\":"); Text(&writer, item->title);
        Raw(&writer, ",\"topic\":"); Text(&writer, item->topic);
        Raw(&writer, ",\"kind\":"); Text(&writer, kind);
        Raw(&writer, ",\"path\":"); Text(&writer, item->path);
        Raw(&writer, ",\"sha256\":"); Text(&writer, digest);
        if (item->example_id[0] != '\0') {
            Raw(&writer, ",\"example\":"); Text(&writer, item->example_id);
        }
        Raw(&writer, "}");
        status = writer.status;
    }
    if (status == UMI_STATUS_OK) {
        Raw(&writer, "]}\n");
        status = writer.status;
    }
    /* Admit our output through the same closed schema used by Studio. The
     * lower compatibility bound is valid for this metadata round trip; it is
     * not a claim about a separately installed application's host version. */
    UmiLearningPack *checked = NULL;
    if (status == UMI_STATUS_OK)
        status = UmiLearningPackCreate(json, writer.length, source->root,
            info->minimum_version, &checked);
    UmiLearningPackDestroy(checked);
    if (status != UMI_STATUS_OK) { free(json); return status; }
    *out_json = (unsigned char *)json;
    *out_length = writer.length;
    return UMI_STATUS_OK;
}
