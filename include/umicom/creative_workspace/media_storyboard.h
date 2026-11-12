/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/creative_workspace/media_storyboard.h
 *
 * PURPOSE:
 *   Plan ordered video scenes using explicit references to creative library assets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CREATIVE_MEDIA_STORYBOARD_H
#define UMICOM_CREATIVE_MEDIA_STORYBOARD_H
#include "umicom/creative_workspace/asset_library.h"
#include "umicom/creative_workspace/asset.h"
#include "umicom/creative_workspace/export.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_MEDIA_STORYBOARD_MAX_SCENES 16U
#define UMI_MEDIA_STORYBOARD_MAX_REFERENCES 8U
#define UMI_MEDIA_STORYBOARD_STORY_CAPACITY 1025U
typedef struct UmiMediaStoryboardScene {
    char title[129];
    char story[UMI_MEDIA_STORYBOARD_STORY_CAPACITY];
    uint32_t duration_seconds; /* 1..600; requested duration, not provider capability. */
} UmiMediaStoryboardScene;
typedef struct UmiMediaStoryboardDraft {
    size_t count;
    UmiMediaStoryboardScene scenes[UMI_MEDIA_STORYBOARD_MAX_SCENES];
} UmiMediaStoryboardDraft;
typedef struct UmiMediaStoryboard UmiMediaStoryboard;
/* Single-owner editing. Initialise drafts to zero. Failed operations preserve
 * the complete draft. References may be unresolved while drafting; Create
 * resolves every @{asset-id} against one captured library inventory. */
UmiStatus UmiMediaStoryboardAppend(UmiMediaStoryboardDraft *draft, const UmiMediaStoryboardScene *scene);
UmiStatus UmiMediaStoryboardUpdate(UmiMediaStoryboardDraft *draft, size_t index, const UmiMediaStoryboardScene *scene);
UmiStatus UmiMediaStoryboardRemove(UmiMediaStoryboardDraft *draft, size_t index);
UmiStatus UmiMediaStoryboardMove(UmiMediaStoryboardDraft *draft, size_t from, size_t to);
/* Creates an immutable, provider-neutral plan. Initialise *out=NULL; all failures
 * preserve the output. Entries supply IDs, labels and kinds, never uploaded bytes
 * or authorisation. An avatar is an image asset with its role described in the
 * story. Tags are case sensitive; malformed, unknown and excess tags are errors.
 * Ordinary @ characters (including email addresses) remain literal text. */
UmiStatus UmiMediaStoryboardCreate(const UmiMediaStoryboardDraft *draft,
    const UmiCreativeAssetLibraryEntry *assets, size_t asset_count, UmiMediaStoryboard **out);
void UmiMediaStoryboardDestroy(UmiMediaStoryboard *plan);
/* Initialise *out={0}. Outputs are UTF-8 text owned by UmiCreativeExportFree.
 * No disk/network/decoder/model calls occur. Instructions describe references;
 * uploading assets and rendering a video require a separate explicit workflow. */
UmiStatus UmiMediaStoryboardInstructions(const UmiMediaStoryboard *plan, UmiCreativeExport *out);
UmiStatus UmiMediaStoryboardSceneInstructions(const UmiMediaStoryboard *plan, size_t index, UmiCreativeExport *out);

/* Editable documents contain only ordered scene inputs. Empty drafts and
 * unresolved/malformed reference tags remain editable text; preparing generation
 * still requires Create to resolve and validate every tag against a library.
 * Active scenes use the same title, story and duration rules as Append/Update.
 * Inactive slots are ignored and are zeroed by import. */
#define UMI_MEDIA_STORYBOARD_DOCUMENT_BYTES (128U * 1024U)
UmiStatus UmiMediaStoryboardDraftValidate(const UmiMediaStoryboardDraft *draft);
/* Version 1 JSON: exact format/version/scenes members; each scene contains only
 * title/story/duration_seconds. Unknown, missing and duplicate fields, future
 * versions, malformed UTF-8 and noninteger durations are refused.
 * Export requires *out=NULL and returns an immutable document asset. Import
 * publishes a complete owned draft only on success, preserving *out on failure.
 * No asset payload, path, derived instructions, provider choice or approval is
 * inferred from this document. Keep inputs/token alive during each call. */
UmiStatus UmiMediaStoryboardDraftExport(const UmiMediaStoryboardDraft *draft,
    const UmiCancellationToken *cancel, UmiCreativeAsset **out);
UmiStatus UmiMediaStoryboardDraftImport(const void *bytes, size_t size,
    const UmiCancellationToken *cancel, UmiMediaStoryboardDraft *out);
/* Worker-only I/O at an explicit ordinary absolute UTF-8 path. Save intentionally
 * replaces the selected destination through Framework's atomic writer, which
 * creates missing parents and uses a same-directory exclusive temporary file.
 * Parent directories must be trusted/stable; this is not a filesystem sandbox
 * or a compare-and-swap save. Concurrent successful saves replace in completion
 * order. Confirm replacement in the caller before invoking Save.
 *
 * Cancellation is checked during preparation and immediately before publication.
 * The atomic write is not interruptible; its actual status is returned even if
 * cancellation arrives after publication starts. Load uses the bounded regular
 * file reader and preserves the existing draft on every failure. Neither call
 * installs, executes, uploads or activates referenced content. */
UmiStatus UmiMediaStoryboardDraftSave(const UmiMediaStoryboardDraft *draft,
    const char *absolute_path, const UmiCancellationToken *cancel);
UmiStatus UmiMediaStoryboardDraftLoad(const char *absolute_path,
    const UmiCancellationToken *cancel, UmiMediaStoryboardDraft *out);

#ifdef __cplusplus
}
#endif
#endif
