/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/design_token.h
 *
 * PURPOSE:
 *   Define typed design tokens that extend the existing generic Umicom theme-token model.
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_DESIGN_TOKEN_H
#define INCLUDE_UMICOM_UI_DESIGN_DESIGN_TOKEN_H

#include "umicom/ui/design/types.h"
#include "umicom/base/value_archive.h"
#include "umicom/ui/design/color.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/design/color.h"
/**
 * Represent the design token data shared with callers of this public contract.
 */
typedef struct UmiDesignToken { char id[UMI_DESIGN_ID_CAPACITY]; UmiDesignValueKind kind; UmiDesignRgba color; double number; int64_t integer; UmiDesignLength length; char text[UMI_DESIGN_TEXT_CAPACITY]; } UmiDesignToken;
/* Initialise a numeric design token. */
UmiStatus umi_design_token_number(UmiDesignToken *token, const char *id, double value);
/* Initialise a colour design token. */
UmiStatus umi_design_token_color(UmiDesignToken *token, const char *id, UmiDesignRgba color);
/* Initialise a bounded text design token. */
UmiStatus umi_design_token_text(UmiDesignToken *token, const char *id, const char *value);
/* Return one when the token identifier and typed value are valid. */
int umi_design_token_valid(const UmiDesignToken *token);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_design_token_archive_encode(const UmiDesignToken *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_design_token_archive_decode(const void *bytes, size_t byte_count,
    UmiDesignToken *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiDesignToken object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_design_token_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiDesignToken *workspace, UmiDesignToken *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_design_token_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiDesignToken *expected, UmiDesignToken *value);


#ifdef __cplusplus
}
#endif

#endif
