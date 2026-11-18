/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/entity_tag.h
 *
 * PURPOSE:
 *   Parse bounded HTTP entity-tag conditions for conditional website responses.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_WEB_ENTITY_TAG_H
#define UMICOM_WEB_ENTITY_TAG_H
#include <stddef.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_WEB_ENTITY_TAG_LIST_LIMIT 8192U
/* Evaluate If-None-Match with HTTP weak comparison (W/"a" matches "a").
 * current_tag is one quoted entity-tag; use NULL/0 when no representation
 * exists. condition NULL/0 means the header was absent, yielding false.
 * A present header must be a valid tag list or a lone *. Opaque tags are
 * case-sensitive; commas inside quoted tags are data. SP/HTAB are allowed
 * around list items. Embedded NUL, CR/LF and malformed tails are refused.
 * All bytes are validated even after a match. Failure preserves out_matches.
 * Buffers are borrowed byte spans; no allocation, storage read or mutation.
 *
 * For GET/HEAD a match permits 304; for other methods the caller applies 412.
 * Evaluate only after authorization and representation selection. This API
 * does not generate ETags or change cache policy. A tag must identify the
 * selected representation including content encoding; timestamps alone are
 * not always sufficient. Existing development previews retain no-store. */
UmiStatus UmiWebIfNoneMatch(const char *condition, size_t condition_size,
    const char *current_tag, size_t current_size, bool *out_matches);
#ifdef __cplusplus
}
#endif
#endif
