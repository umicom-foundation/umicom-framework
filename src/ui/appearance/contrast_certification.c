/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/contrast_certification.c
 *
 * PURPOSE:
 *   Certify measured Design-System contrast ratios against policy thresholds without duplicating colour science.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/contrast_certification.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_contrast_certification_init(UmiAppearanceContrastCertification *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->target_id,sizeof item->target_id,"text.primary");
    item->measured_ratio=7.0;
    item->required_ratio=4.5;
    item->passed=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_contrast_certification_is_valid(const UmiAppearanceContrastCertification *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->target_id, '\0', sizeof(item->target_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->target_id) && item->measured_ratio >= 1.0 && item->required_ratio >= 1.0);
}
/*
 * Provide the appearance contrast certification evaluate operation used by this module and
 * its client applications.
 */
UmiStatus umi_appearance_contrast_certification_evaluate(UmiAppearanceContrastCertification *item,double measured,double required){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL||measured<1.0||required<1.0)return UMI_STATUS_INVALID_ARGUMENT;item->measured_ratio=measured;item->required_ratio=required;item->passed=measured>=required;return UMI_STATUS_OK;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceContrastCertificationArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xec7097a34179bd5f);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceContrastCertification *)0)->target_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceContrastCertificationArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceContrastCertification *)0)->target_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceContrastCertificationArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceContrastCertification *value)
{
    UmiArchiveWriteText(writer, value->target_id, sizeof(value->target_id));
    UmiArchiveWriteDouble(writer, value->measured_ratio);
    UmiArchiveWriteDouble(writer, value->required_ratio);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->passed);
}
static void UmiAppearanceContrastCertificationArchiveRead(UmiArchiveReader *reader, UmiAppearanceContrastCertification *value)
{
    UmiArchiveReadText(reader, value->target_id, sizeof(value->target_id));
    value->measured_ratio = UmiArchiveReadDouble(reader);
    value->required_ratio = UmiArchiveReadDouble(reader);
    value->passed = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiAppearanceContrastCertificationArchiveValidate(const UmiAppearanceContrastCertification *value)
{
    return umi_appearance_contrast_certification_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_contrast_certification_archive_encode, umi_appearance_contrast_certification_archive_decode,
    UmiAppearanceContrastCertification, UmiAppearanceContrastCertificationArchiveSchema, UmiAppearanceContrastCertificationArchiveBound, UmiAppearanceContrastCertificationArchiveWrite, UmiAppearanceContrastCertificationArchiveRead, UmiAppearanceContrastCertificationArchiveValidate)
