/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/creative_workspace/media_brief.h
 *
 * PURPOSE:
 *   Prepare bounded creation briefs from explicit source excerpts for media tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CREATIVE_WORKSPACE_MEDIA_BRIEF_H
#define UMICOM_CREATIVE_WORKSPACE_MEDIA_BRIEF_H
#include "umicom/base/status.h"
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_MEDIA_BRIEF_PROMPT_CAPACITY 4097U
typedef enum UmiMediaBriefKind {
    UMI_MEDIA_BRIEF_STORYBOARD = 0,
    UMI_MEDIA_BRIEF_IMAGE,
    UMI_MEDIA_BRIEF_VIDEO,
    UMI_MEDIA_BRIEF_MUSIC
} UmiMediaBriefKind;
typedef struct UmiMediaBriefDraft {
    UmiMediaBriefKind kind;
    char title[129];
    char source[2049];
    char direction[513];
} UmiMediaBriefDraft;
typedef struct UmiMediaBrief UmiMediaBrief;
/* All limits are UTF-8 bytes, excluding the terminator. Title and source must
 * contain visible text. Direction is optional. Tab, CR and LF are allowed;
 * malformed UTF-8 and other ASCII control characters are rejected.
 * Create copies the draft and prepares a deterministic prompt, with no file,
 * model or network access. It does not summarise or generate media. A book is
 * processed as deliberate excerpts, never silently cut to fit a provider.
 * On failure *out is NULL. The returned prompt is borrowed until Destroy. */
UmiStatus UmiMediaBriefCreate(const UmiMediaBriefDraft *draft, UmiMediaBrief **out);
const char *UmiMediaBriefPrompt(const UmiMediaBrief *brief);
void UmiMediaBriefDestroy(UmiMediaBrief *brief);
#ifdef __cplusplus
}
#endif
#endif
