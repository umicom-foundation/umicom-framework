/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/backtrace_text.c
 *
 * PURPOSE:
 *   Render bounded debugger observations as plain text with explicit source-path policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "backtrace_private.h"
#include "umicom/document/text_encoding.h"
#include <stdio.h>
#include <string.h>
typedef struct TraceWriter {
  char *out;
  size_t used;
} TraceWriter;
static void Write(TraceWriter *writer, const char *text, size_t bytes) {
  if (writer->out != NULL)
    memcpy(writer->out + writer->used, text, bytes);
  writer->used += bytes;
}
static void Literal(TraceWriter *writer, const char *text) { Write(writer, text, strlen(text)); }
static bool TextValid(const char *text, size_t capacity) {
  const char *end = memchr(text, '\0', capacity);
  return end != NULL &&
         umi_document_utf8_validate((const unsigned char *)text, (size_t)(end - text), NULL);
}
static void Escaped(TraceWriter *writer, const char *text) {
  const char hex[] = "0123456789abcdef";
  for (const unsigned char *p = (const unsigned char *)text; *p; ++p) {
    if (*p == '\\')
      Literal(writer, "\\\\");
    else if (*p < 32U || *p == 127U) {
      char escaped[] = {'\\', 'x', hex[*p >> 4U], hex[*p & 15U]};
      Write(writer, escaped, sizeof escaped);
    } else
      Write(writer, (const char *)p, 1U);
  }
}
static void Number(TraceWriter *writer, size_t value) {
  char text[32];
  (void)snprintf(text, sizeof text, "%zu", value);
  Literal(writer, text);
}
static void Render(TraceWriter *writer, const UmiDebugBacktrace *trace,
                   UmiDebugBacktracePaths paths) {
  Literal(writer, "Captured call stack (retained frames; not a live process snapshot)\nThread: ");
  Escaped(writer, trace->summary.thread.id);
  if (trace->summary.thread.name[0]) {
    Literal(writer, " (");
    Escaped(writer, trace->summary.thread.name);
    Literal(writer, ")");
  }
  Literal(writer, "\nFrames: ");
  Number(writer, trace->summary.frame_count);
  Literal(writer, "\n");
  for (size_t i = 0U; i < trace->summary.frame_count; ++i) {
    const UmiDebugStackFrameSnapshot *frame = &trace->frames[i];
    Literal(writer, "#");
    Number(writer, i);
    Literal(writer, " ");
    Escaped(writer, frame->name[0] ? frame->name : "(unnamed)");
    if (frame->source_uri[0] && paths != UMI_DEBUG_BACKTRACE_OMIT_PATHS) {
      const char *source = frame->source_uri;
      if (paths == UMI_DEBUG_BACKTRACE_FILENAMES)
        for (const char *p = source; *p; ++p)
          if (*p == '/' || *p == '\\')
            source = p + 1;
      Literal(writer, " at ");
      Escaped(writer, source[0] ? source : "(unnamed source)");
      if (frame->line) {
        Literal(writer, ":");
        Number(writer, frame->line);
        if (frame->column) {
          Literal(writer, ":");
          Number(writer, frame->column);
        }
      }
    } else if (frame->source_uri[0])
      Literal(writer, " [source omitted]");
    else
      Literal(writer, " [source unavailable]");
    Literal(writer, "\n");
  }
}
UmiStatus UmiDebugBacktraceFormat(const UmiDebugBacktrace *trace, UmiDebugBacktracePaths paths,
                                  char *out, size_t capacity, size_t *required) {
  if (trace == NULL || required == NULL || (out == NULL && capacity != 0U) ||
      paths < UMI_DEBUG_BACKTRACE_OMIT_PATHS || paths > UMI_DEBUG_BACKTRACE_FULL_PATHS)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (!TextValid(trace->summary.thread.id, sizeof trace->summary.thread.id) ||
      !TextValid(trace->summary.thread.name, sizeof trace->summary.thread.name))
    return UMI_STATUS_PARSE_ERROR;
  for (size_t i = 0U; i < trace->summary.frame_count; ++i)
    if (!TextValid(trace->frames[i].name, sizeof trace->frames[i].name) ||
        !TextValid(trace->frames[i].source_uri, sizeof trace->frames[i].source_uri))
      return UMI_STATUS_PARSE_ERROR;
  /* The opaque capture bounds both frame count and every field. Measure first
   * so a failed destination-size check cannot publish a partial report. */
  TraceWriter measured = {0};
  Render(&measured, trace, paths);
  *required = measured.used;
  if (out == NULL)
    return UMI_STATUS_OK;
  if (capacity <= measured.used)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  TraceWriter writer = {out, 0U};
  Render(&writer, trace, paths);
  out[writer.used] = '\0';
  return UMI_STATUS_OK;
}
