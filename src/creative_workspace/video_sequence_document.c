/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/video_sequence_document.c
 * PURPOSE: Persist editable local video recipes with bounded strict JSON.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/mcp/json.h"
#include "umicom/creative_workspace/video_sequence.h"
#include "umicom/language_runtime/json_tree.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct Writer {
  char *bytes;
  size_t used;
  UmiStatus status;
} Writer;
static void Append(Writer *writer, const char *format, ...) {
  if (writer->status != UMI_STATUS_OK)
    return;
  va_list arguments;
  va_start(arguments, format);
  size_t room = UMI_VIDEO_SEQUENCE_DOCUMENT_BYTES - writer->used;
  int count = vsnprintf(writer->bytes + writer->used, room, format, arguments);
  va_end(arguments);
  if (count < 0 || (size_t)count >= room)
    writer->status = UMI_STATUS_CAPACITY_EXCEEDED;
  else
    writer->used += (size_t)count;
}
static void Quoted(Writer *writer, const char *text) {
  if (writer->status != UMI_STATUS_OK)
    return;
  writer->status = umi_ai_mcp_json_escape_string(text, writer->bytes + writer->used,
                                                 UMI_VIDEO_SEQUENCE_DOCUMENT_BYTES - writer->used);
  if (writer->status == UMI_STATUS_OK)
    writer->used += strlen(writer->bytes + writer->used);
}
UmiStatus UmiVideoSequenceExport(const UmiVideoSequence *plan, const UmiCancellationToken *cancel,
                                 UmiCreativeAsset **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (*out != NULL)
    return UMI_STATUS_INVALID_STATE;
  UmiStatus status = UmiVideoSequenceValidate(plan);
  if (status != UMI_STATUS_OK)
    return status;
  if (umi_cancellation_token_is_requested(cancel))
    return UMI_STATUS_CANCELLED;
  Writer writer = {calloc(UMI_VIDEO_SEQUENCE_DOCUMENT_BYTES, 1U), 0U, UMI_STATUS_OK};
  if (writer.bytes == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  Append(&writer, "{\"format\":\"umicom-video-sequence\",\"title\":");
  Quoted(&writer, plan->title);
  Append(&writer, ",\"width\":%u,\"height\":%u,\"clips\":[", plan->width, plan->height);
  for (size_t i = 0U; i < plan->clip_count; ++i) {
    const UmiVideoSequenceClip *clip = &plan->clips[i];
    Append(&writer, "%s{\"id\":", i == 0U ? "" : ",");
    Quoted(&writer, clip->id);
    Append(&writer, ",\"path\":");
    Quoted(&writer, clip->path);
    Append(&writer, ",\"begin_ms\":%u,\"end_ms\":%u}", clip->begin_ms, clip->end_ms);
  }
  Append(&writer, "]}\n");
  status = writer.status;
  if (status == UMI_STATUS_OK)
    status = UmiCreativeAssetCapture(plan->title, UMI_CREATIVE_ASSET_DOCUMENT, writer.bytes,
                                     writer.used, cancel, out);
  free(writer.bytes);
  return status;
}
static UmiStatus Text(const UmiJsonTree *tree, int object, const char *key, char *out,
                      size_t capacity) {
  int node = -1;
  UmiStatus status = UmiJsonTreeMember(tree, object, key, &node);
  return status == UMI_STATUS_OK ? UmiJsonTreeText(tree, node, out, capacity) : status;
}
static UmiStatus Number(const UmiJsonTree *tree, int object, const char *key, uint32_t *out) {
  int node = -1;
  int64_t value = 0;
  UmiStatus status = UmiJsonTreeMember(tree, object, key, &node);
  if (status == UMI_STATUS_OK)
    status = UmiJsonTreeInteger(tree, node, &value);
  if (status == UMI_STATUS_OK && (value < 0 || value > UINT32_MAX))
    status = UMI_STATUS_PARSE_ERROR;
  if (status == UMI_STATUS_OK)
    *out = (uint32_t)value;
  return status;
}
/* Exact shapes reject forgotten fields and future editing operations rather
 * than importing a recipe that would silently lose the author's intent. */
static UmiStatus Placement(const UmiJsonTree *tree, int object, UmiVideoSequenceClip *out) {
  if (UmiJsonTreeKind(tree, object) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT ||
      UmiJsonTreeCount(tree, object) != 4U)
    return UMI_STATUS_PARSE_ERROR;
  UmiStatus status = Text(tree, object, "id", out->id, sizeof(out->id));
  if (status == UMI_STATUS_OK)
    status = Text(tree, object, "path", out->path, sizeof(out->path));
  if (status == UMI_STATUS_OK)
    status = Number(tree, object, "begin_ms", &out->begin_ms);
  if (status == UMI_STATUS_OK)
    status = Number(tree, object, "end_ms", &out->end_ms);
  return status;
}
UmiStatus UmiVideoSequenceImport(const void *bytes, size_t size, const UmiCancellationToken *cancel,
                                 UmiVideoSequence *out) {
  if (bytes == NULL || out == NULL || size == 0U)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiJsonTree *tree = NULL;
  UmiJsonTreeLimits limits = {UMI_VIDEO_SEQUENCE_DOCUMENT_BYTES, 512U, 4U};
  UmiStatus status = UmiJsonTreeCreate(bytes, size, &limits, cancel, &tree);
  UmiVideoSequence plan = {0};
  char format[64];
  int clips = -1;
  /* Exact required members prevent accepting a newer recipe while silently
   * throwing away unknown operations. Each lookup also rejects duplicate keys. */
  if (status == UMI_STATUS_OK && (UmiJsonTreeKind(tree, 0) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT ||
                                  UmiJsonTreeCount(tree, 0) != 5U))
    status = UMI_STATUS_PARSE_ERROR;
  if (status == UMI_STATUS_OK)
    status = Text(tree, 0, "format", format, sizeof(format));
  if (status == UMI_STATUS_OK && strcmp(format, "umicom-video-sequence") != 0)
    status = UMI_STATUS_PARSE_ERROR;
  if (status == UMI_STATUS_OK)
    status = Text(tree, 0, "title", plan.title, sizeof(plan.title));
  if (status == UMI_STATUS_OK)
    status = Number(tree, 0, "width", &plan.width);
  if (status == UMI_STATUS_OK)
    status = Number(tree, 0, "height", &plan.height);
  if (status == UMI_STATUS_OK)
    status = UmiJsonTreeMember(tree, 0, "clips", &clips);
  if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, clips) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
    status = UMI_STATUS_PARSE_ERROR;
  if (status == UMI_STATUS_OK && UmiJsonTreeCount(tree, clips) > UMI_VIDEO_SEQUENCE_MAX_CLIPS)
    status = UMI_STATUS_CAPACITY_EXCEEDED;
  for (int node = status == UMI_STATUS_OK ? UmiJsonTreeFirst(tree, clips) : -1;
       node >= 0 && status == UMI_STATUS_OK; node = UmiJsonTreeNext(tree, node)) {
    UmiVideoSequenceClip placement = {0};
    status = Placement(tree, node, &placement);
    if (status == UMI_STATUS_OK)
      status = UmiVideoSequencePut(&plan, &placement, false);
  }
  if (status == UMI_STATUS_OK)
    status = UmiVideoSequenceValidate(&plan);
  if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
    status = UMI_STATUS_CANCELLED;
  if (status == UMI_STATUS_OK)
    *out = plan;
  UmiJsonTreeDestroy(tree);
  return status;
}
