/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/contrast_evidence.c
 *
 * PURPOSE:
 *   Persist auditable foreground/background token and ratio evidence for conformance reports.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/contrast_evidence.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_contrast_evidence_init(UmiAppearanceContrastEvidence *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->evidence_id,sizeof item->evidence_id,"contrast.text.primary");
    (void)umi_appearance_copy_text(item->foreground_token,sizeof item->foreground_token,"text.primary");
    (void)umi_appearance_copy_text(item->background_token,sizeof item->background_token,"surface.background");
    item->ratio=7.0;
    item->passed=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_contrast_evidence_is_valid(const UmiAppearanceContrastEvidence *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->evidence_id, '\0', sizeof(item->evidence_id)) == NULL) return 0;
    if (memchr(item->foreground_token, '\0', sizeof(item->foreground_token)) == NULL) return 0;
    if (memchr(item->background_token, '\0', sizeof(item->background_token)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->evidence_id) && item->ratio >= 1.0);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceContrastEvidenceArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x5a2cef01fd11cdc0);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceContrastEvidence *)0)->evidence_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceContrastEvidence *)0)->foreground_token)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceContrastEvidence *)0)->background_token)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceContrastEvidenceArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceContrastEvidence *)0)->evidence_id) - 1U +
        8U + sizeof(((UmiAppearanceContrastEvidence *)0)->foreground_token) - 1U +
        8U + sizeof(((UmiAppearanceContrastEvidence *)0)->background_token) - 1U +
        8U +
        8U;
}
static void UmiAppearanceContrastEvidenceArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceContrastEvidence *value)
{
    UmiArchiveWriteText(writer, value->evidence_id, sizeof(value->evidence_id));
    UmiArchiveWriteText(writer, value->foreground_token, sizeof(value->foreground_token));
    UmiArchiveWriteText(writer, value->background_token, sizeof(value->background_token));
    UmiArchiveWriteDouble(writer, value->ratio);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->passed);
}
static void UmiAppearanceContrastEvidenceArchiveRead(UmiArchiveReader *reader, UmiAppearanceContrastEvidence *value)
{
    UmiArchiveReadText(reader, value->evidence_id, sizeof(value->evidence_id));
    UmiArchiveReadText(reader, value->foreground_token, sizeof(value->foreground_token));
    UmiArchiveReadText(reader, value->background_token, sizeof(value->background_token));
    value->ratio = UmiArchiveReadDouble(reader);
    value->passed = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiAppearanceContrastEvidenceArchiveValidate(const UmiAppearanceContrastEvidence *value)
{
    return umi_appearance_contrast_evidence_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_contrast_evidence_archive_encode, umi_appearance_contrast_evidence_archive_decode,
    UmiAppearanceContrastEvidence, UmiAppearanceContrastEvidenceArchiveSchema, UmiAppearanceContrastEvidenceArchiveBound, UmiAppearanceContrastEvidenceArchiveWrite, UmiAppearanceContrastEvidenceArchiveRead, UmiAppearanceContrastEvidenceArchiveValidate)
