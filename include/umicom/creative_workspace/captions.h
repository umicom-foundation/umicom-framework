/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/creative_workspace/captions.h
 * PURPOSE: Own and edit timed captions independently of a provider, file or user interface.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_CREATIVE_WORKSPACE_CAPTIONS_H
#define UMICOM_CREATIVE_WORKSPACE_CAPTIONS_H
#include "umicom/creative_workspace/asset.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CAPTION_CUE_LIMIT 512U
#define UMI_CAPTION_TEXT_CAPACITY 2049U
#define UMI_CAPTION_DOCUMENT_LIMIT (2U * 1024U * 1024U)
#define UMI_CAPTION_TIME_LIMIT_MS 359999999U
typedef struct UmiCaptionTrack UmiCaptionTrack;
typedef struct UmiCaptionCue {
    uint32_t begin_ms, end_ms;
    char text[UMI_CAPTION_TEXT_CAPACITY];
} UmiCaptionCue;
typedef struct UmiCaptionInfo {
    char title[UMI_CREATIVE_LABEL_CAPACITY];
    size_t cue_count;
    uint32_t end_ms;
} UmiCaptionInfo;
typedef enum UmiCaptionFormat {
    UMI_CAPTION_SRT = 1,
    UMI_CAPTION_WEBVTT = 2
} UmiCaptionFormat;
/**
 * Copy a complete caption track. Initialise *out to NULL; all failures leave
 * it unchanged. Each cue has begin < end, within TIME_LIMIT, and nondecreasing
 * begin times. Overlap is intentional and retained, including equal starts.
 * Text is UTF-8 with LF between nonblank lines, at most 2048 bytes per cue.
 * Markup characters <, > and & are refused so exports have plain-text meaning.
 * Empty tracks are valid. The immutable track owns every byte; callers keep it
 * alive while reading. No file, provider, device or database I/O occurs.
 */
UmiStatus UmiCaptionTrackCreate(const char *title, const UmiCaptionCue *cues,
    size_t count, UmiCaptionTrack **out);
/** Release one owned track. NULL is harmless; do not race readers. */
void UmiCaptionTrackDestroy(UmiCaptionTrack *track);
/** Copy title, cue count and latest end time; failure preserves the output. */
UmiStatus UmiCaptionTrackInspect(const UmiCaptionTrack *track, UmiCaptionInfo *out);
/** Copy one zero-based cue, including overlaps; failure preserves the output. */
UmiStatus UmiCaptionTrackCue(const UmiCaptionTrack *track, size_t index, UmiCaptionCue *out);
/**
 * Import the plain-text SRT subset. Accept UTF-8, an optional initial BOM,
 * LF or CRLF, increasing positive numeric identifiers, and exact
 * HH:MM:SS,mmm --> HH:MM:SS,mmm timing lines. Empty files are valid.
 * Blank lines separate cues. Formatting, cue positions, lone CR and malformed
 * UTF-8 are refused rather than silently discarded. Export renumbers from one.
 * The input is length-delimited and must not contain NUL. No partial import.
 */
UmiStatus UmiCaptionTrackParseSrt(const char *title, const void *bytes,
    size_t length, UmiCaptionTrack **out);
/**
 * Produce an independently owned shifted track. No cue is silently clipped:
 * any negative start or end beyond TIME_LIMIT refuses the whole operation.
 * The signed offset may be INT64_MIN; arithmetic remains checked.
 */
UmiStatus UmiCaptionTrackShift(const UmiCaptionTrack *track, int64_t offset_ms,
    UmiCaptionTrack **out);
/**
 * Keep intersections with [begin_ms,end_ms), optionally subtracting begin_ms
 * for a new clip timeline. Boundary-touching cues do not survive. Other cue
 * text and overlap are retained. A clip with no captions is a valid empty track.
 */
UmiStatus UmiCaptionTrackClip(const UmiCaptionTrack *track, uint32_t begin_ms,
    uint32_t end_ms, bool rebase, UmiCaptionTrack **out);
/** Replace one cue in a new track; validate timing against both neighbours. */
UmiStatus UmiCaptionTrackReplace(const UmiCaptionTrack *track, size_t index,
    const UmiCaptionCue *cue, UmiCaptionTrack **out);
/** Insert before index (count appends). Existing tracks remain unchanged. */
UmiStatus UmiCaptionTrackInsert(const UmiCaptionTrack *track, size_t index,
    const UmiCaptionCue *cue, UmiCaptionTrack **out);
/** Remove one cue in a new track; removing the final cue creates an empty track. */
UmiStatus UmiCaptionTrackRemove(const UmiCaptionTrack *track, size_t index,
    UmiCaptionTrack **out);
/**
 * Build an immutable asset containing canonical UTF-8 SRT or plain WebVTT.
 * Use UmiCreativeAssetWriteNew for explicit file output. No source is replaced.
 * Initialise *out=NULL. Cancellation preserves it. The caller owns the result.
 */
UmiStatus UmiCaptionTrackExport(const UmiCaptionTrack *track, UmiCaptionFormat format,
    const UmiCancellationToken *cancel, UmiCreativeAsset **out);
#ifdef __cplusplus
}
#endif
#endif
