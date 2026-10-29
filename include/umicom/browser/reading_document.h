/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/browser/reading_document.h
 *
 * PURPOSE:
 *   Define an owned bounded reading projection without executing document content.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROWSER_READING_DOCUMENT_H
#define UMICOM_BROWSER_READING_DOCUMENT_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_BROWSER_READING_SOURCE_MAX (1024U * 1024U)
#define UMI_BROWSER_READING_TEXT_MAX 65536U
#define UMI_BROWSER_READING_SPANS_MAX 512U
#define UMI_BROWSER_READING_LINKS_MAX 128U
#define UMI_BROWSER_READING_ANCHORS_MAX 128U
#define UMI_BROWSER_READING_TARGET_CAPACITY 1024U
#define UMI_BROWSER_READING_ANCHOR_CAPACITY 128U
typedef struct UmiBrowserReadingDocument UmiBrowserReadingDocument;
typedef enum UmiBrowserReadingKind {
    UMI_BROWSER_READING_HEADING = 1,
    UMI_BROWSER_READING_CODE = 2,
    UMI_BROWSER_READING_STRONG = 3,
    UMI_BROWSER_READING_EMPHASIS = 4,
    UMI_BROWSER_READING_IMAGE_DESCRIPTION = 5
} UmiBrowserReadingKind;
typedef struct UmiBrowserReadingSpan {
    UmiBrowserReadingKind kind;
    size_t byte_start;
    size_t byte_length;
    unsigned int heading_level;
} UmiBrowserReadingSpan;
typedef struct UmiBrowserReadingLink {
    size_t byte_start;
    size_t byte_length;
    char target[UMI_BROWSER_READING_TARGET_CAPACITY];
} UmiBrowserReadingLink;
/* Image references remain inert document data. Their text range covers the
 * retained alternative-text description, including its displayed prefix.
 * target_status is OK only for a complete nonempty decoded src attribute;
 * missing/empty, ambiguous and oversized values remain visible descriptions
 * with NOT_FOUND, INVALID_ARGUMENT or CAPACITY_EXCEEDED metadata respectively.
 * Even an OK target still needs the host's manifest and resource-kind policy. */
typedef struct UmiBrowserReadingImage {
    size_t byte_start, byte_length;
    char target[UMI_BROWSER_READING_TARGET_CAPACITY];
    UmiStatus target_status;
    int description_missing, decorative;
} UmiBrowserReadingImage;
typedef struct UmiBrowserReadingSummary {
    size_t source_bytes, text_bytes, spans, links, anchors;
    size_t image_descriptions, missing_image_descriptions;
} UmiBrowserReadingSummary;

/* A bounded reading projection, not an HTML engine or sanitizer. It never
 * opens files, follows links, executes scripts, evaluates CSS or fetches images.
 * Headings, emphasis, preformatted code, list markers and table-cell separators
 * are retained; images become explicit alternative-text descriptions.
 *
 * Input is byte-oriented: NUL and ASCII control bytes other than tab/newlines
 * are refused. Frontends must validate UTF-8 before displaying bytes. Named
 * entities amp/lt/gt/quot/apos/nbsp and valid numeric scalar entities are decoded.
 * Unknown entities remain literal. Script/style/head/template/SVG/iframe/object
 * content is omitted. Unsupported ordinary tags retain their visible text.
 * Styling tags must nest within 64 levels; malformed/incomplete supported markup
 * is refused rather than producing misleading ranges. No partial model escapes
 * on failure or capacity exhaustion. The caller may retain a plain-text fallback.
 *
 * Result owns all text and metadata. out_document is cleared on every refusal.
 * A successful model is immutable and may outlive or be read independently of
 * its source buffer. Destroy only after all users have finished reading it. */
UmiStatus UmiBrowserReadingDocumentCreate(const void *html, size_t bytes,
    UmiBrowserReadingDocument **out_document);
void UmiBrowserReadingDocumentDestroy(UmiBrowserReadingDocument *document);
UmiStatus UmiBrowserReadingDocumentSummary(const UmiBrowserReadingDocument *document,
    UmiBrowserReadingSummary *out_summary);
/* Borrowed NUL-terminated text, valid until Destroy. NULL for NULL owner. */
const char *UmiBrowserReadingDocumentText(const UmiBrowserReadingDocument *document);
/* Metadata output is preserved on refusal; indices are zero-based.
 * Link targets are data, not approved navigations. The host must apply its
 * manifest/root/network policy before an explicit user action follows one. */
UmiStatus UmiBrowserReadingDocumentSpanAt(const UmiBrowserReadingDocument *document,
    size_t index, UmiBrowserReadingSpan *out_span);
UmiStatus UmiBrowserReadingDocumentLinkAt(const UmiBrowserReadingDocument *document,
    size_t index, UmiBrowserReadingLink *out_link);
/* There is one copied image entry per summary.image_descriptions, at most
 * UMI_BROWSER_READING_SPANS_MAX. Source attributes are never fetched. A malformed
 * src does not discard an otherwise readable guide or expose a partial target.
 * Missing indices and invalid arguments preserve output. */
UmiStatus UmiBrowserReadingDocumentImageAt(const UmiBrowserReadingDocument *document,
    size_t index, UmiBrowserReadingImage *out_image);
/* Exact decoded ID lookup; duplicate nonempty IDs are refused during admission.
 * A missing anchor never silently jumps to the beginning of a document. */
UmiStatus UmiBrowserReadingDocumentAnchor(const UmiBrowserReadingDocument *document,
    const char *identifier, size_t *out_byte_offset);
#ifdef __cplusplus
}
#endif
#endif
