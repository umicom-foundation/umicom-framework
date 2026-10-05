/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/focus_evidence.c
 *
 * PURPOSE:
 *   Record keyboard reachability and visible-focus evidence for a semantic interactive element.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/focus_evidence.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_focus_evidence_init(UmiAppearanceFocusEvidence *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->element_id,sizeof item->element_id,"button.submit");
    item->keyboard_reachable=true;
    item->visible_indicator=true;
    item->order_defined=true;
    item->passed=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_focus_evidence_is_valid(const UmiAppearanceFocusEvidence *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->element_id, '\0', sizeof(item->element_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->element_id) && item->passed == (item->keyboard_reachable && item->visible_indicator && item->order_defined));
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceFocusEvidenceArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xd2a45dc01e1b9a77);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceFocusEvidence *)0)->element_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceFocusEvidenceArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceFocusEvidence *)0)->element_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceFocusEvidenceArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceFocusEvidence *value)
{
    UmiArchiveWriteText(writer, value->element_id, sizeof(value->element_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->keyboard_reachable);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->visible_indicator);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->order_defined);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->passed);
}
static void UmiAppearanceFocusEvidenceArchiveRead(UmiArchiveReader *reader, UmiAppearanceFocusEvidence *value)
{
    UmiArchiveReadText(reader, value->element_id, sizeof(value->element_id));
    value->keyboard_reachable = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->visible_indicator = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->order_defined = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->passed = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiAppearanceFocusEvidenceArchiveValidate(const UmiAppearanceFocusEvidence *value)
{
    return umi_appearance_focus_evidence_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_focus_evidence_archive_encode, umi_appearance_focus_evidence_archive_decode,
    UmiAppearanceFocusEvidence, UmiAppearanceFocusEvidenceArchiveSchema, UmiAppearanceFocusEvidenceArchiveBound, UmiAppearanceFocusEvidenceArchiveWrite, UmiAppearanceFocusEvidenceArchiveRead, UmiAppearanceFocusEvidenceArchiveValidate)
