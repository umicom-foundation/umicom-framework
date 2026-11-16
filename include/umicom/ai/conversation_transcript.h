/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/conversation_transcript.h
 *
 * PURPOSE:
 *   Format explicitly selected conversation messages as bounded literal Markdown fragments.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_CONVERSATION_TRANSCRIPT_H
#define UMICOM_AI_CONVERSATION_TRANSCRIPT_H
#include "umicom/ai/conversation_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_TRANSCRIPT_SYSTEM 1U
#define UMI_AI_TRANSCRIPT_USER 2U
#define UMI_AI_TRANSCRIPT_ASSISTANT 4U
#define UMI_AI_TRANSCRIPT_TOOL 8U
#define UMI_AI_TRANSCRIPT_DIALOGUE (UMI_AI_TRANSCRIPT_USER | UMI_AI_TRANSCRIPT_ASSISTANT)
#define UMI_AI_TRANSCRIPT_ALL 15U
#define UMI_AI_TRANSCRIPT_MAX_BYTES (1024U * 1024U)

typedef struct UmiAiConversationTranscriptOptions {
    size_t first_message;
    size_t message_count;
    unsigned int roles;
    int include_message_names;
} UmiAiConversationTranscriptOptions;

/* Produce a Markdown content fragment, not a branded document or archive file.
 * The host composes approved logo/resources and checks disclosure/persistence
 * policy before saving, copying or sending the result. No I/O, model request,
 * role execution, permission change or allocation occurs here.
 *
 * Select a zero-based range within the active message extent and a nonzero mask
 * of the four named role flags. DIALOGUE excludes system/tool messages. An empty
 * range is valid; an overflowing/out-of-range extent or unknown mask is refused.
 * Nonzero include_message_names exports each selected message's name. Title and
 * classification are always included; provider/model/workspace/session IDs,
 * timestamps, approval flags and hidden bytes after terminators are omitted.
 * Original one-based message ordinals are retained after filtering.
 *
 * Reuse complete record validation, then validate UTF-8 only in emitted title,
 * message names and text. Malformed encoding returns PARSE_ERROR. C0 controls
 * other than TAB/LF/CR, and DEL, return INVALID_ARGUMENT. Other Unicode scalars
 * are preserved without normalization; this is not a visual-spoofing check.
 * Every caller-controlled text field uses a tilde fence longer than any tilde
 * run it contains. A CommonMark-compatible viewer treats that text literally.
 * This is a reading projection, not a lossless/importable record or HTML.
 *
 * Byte counts exclude the trailing NUL. Measure first, allocate bytes + 1, and
 * retain the same immutable inputs through rendering. Re-measure after an edit.
 * Output buffer, byte-count object, record and options must not overlap; the
 * caller owns their lifetimes and synchronization. Every refused call preserves
 * outputs. Render writes only bytes + 1, leaving excess capacity untouched.
 * Output is bounded by MAX_BYTES, excluding its NUL; capacity failure never
 * truncates content. Query required capacity using Measure, not a NULL buffer.
 */
UmiStatus UmiAiConversationTranscriptMeasure(const UmiAiConversationRecord *record,
    const UmiAiConversationTranscriptOptions *options, size_t *out_bytes);
UmiStatus UmiAiConversationTranscriptMarkdown(const UmiAiConversationRecord *record,
    const UmiAiConversationTranscriptOptions *options, char *output,
    size_t capacity, size_t *out_bytes);

#ifdef __cplusplus
}
#endif
#endif
