/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/media_spec.h
 *
 * PURPOSE:
 *   Define image, audio and video media presentation and transport-control semantics.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_MEDIA_SPEC_H
#define INCLUDE_UMICOM_UI_DESIGN_MEDIA_SPEC_H

#include "umicom/ui/design/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named design media kind values accepted by this public contract.
 */
typedef enum UmiDesignMediaKind { UMI_DESIGN_MEDIA_IMAGE=1, UMI_DESIGN_MEDIA_AUDIO=2, UMI_DESIGN_MEDIA_VIDEO=3 } UmiDesignMediaKind;
/**
 * Represent the design media spec data shared with callers of this public contract.
 */
typedef struct UmiDesignMediaSpec { UmiDesignMediaKind kind; int controls; int autoplay; int loop; int preserve_aspect; } UmiDesignMediaSpec;
/* Initialise a semantic media-surface specification. */
UmiStatus umi_design_media_spec_init(UmiDesignMediaSpec *spec, UmiDesignMediaKind kind, int controls, int autoplay, int loop, int preserve_aspect);
/* Return one when media kind and behaviour are valid. */
int umi_design_media_spec_valid(const UmiDesignMediaSpec *spec);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_design_media_spec_archive_encode(const UmiDesignMediaSpec *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_design_media_spec_archive_decode(const void *bytes, size_t byte_count,
    UmiDesignMediaSpec *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiDesignMediaSpec object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_design_media_spec_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiDesignMediaSpec *workspace, UmiDesignMediaSpec *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_design_media_spec_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiDesignMediaSpec *expected, UmiDesignMediaSpec *value);


#ifdef __cplusplus
}
#endif

#endif
