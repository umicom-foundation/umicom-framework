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
#ifdef __cplusplus
}
#endif
#endif
