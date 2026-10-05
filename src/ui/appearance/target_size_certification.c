/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/target_size_certification.c
 *
 * PURPOSE:
 *   Certify resolved interactive target dimensions against modality-specific accessibility policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/target_size_certification.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_target_size_certification_init(UmiAppearanceTargetSizeCertification *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->target_id,sizeof item->target_id,"button.submit");
    item->width_dp=44.0;
    item->height_dp=44.0;
    item->required_width_dp=44.0;
    item->required_height_dp=44.0;
    item->passed=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_target_size_certification_is_valid(const UmiAppearanceTargetSizeCertification *item) {
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
    return (umi_appearance_id_valid(item->target_id) && item->width_dp > 0.0 && item->height_dp > 0.0);
}
/*
 * Provide the appearance target size certification evaluate operation used by this module
 * and its client applications.
 */
void umi_appearance_target_size_certification_evaluate(UmiAppearanceTargetSizeCertification *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item!=NULL)item->passed=item->width_dp>=item->required_width_dp&&item->height_dp>=item->required_height_dp;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceTargetSizeCertificationArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x5048219aee6270f1);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceTargetSizeCertification *)0)->target_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceTargetSizeCertificationArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceTargetSizeCertification *)0)->target_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceTargetSizeCertificationArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceTargetSizeCertification *value)
{
    UmiArchiveWriteText(writer, value->target_id, sizeof(value->target_id));
    UmiArchiveWriteDouble(writer, value->width_dp);
    UmiArchiveWriteDouble(writer, value->height_dp);
    UmiArchiveWriteDouble(writer, value->required_width_dp);
    UmiArchiveWriteDouble(writer, value->required_height_dp);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->passed);
}
static void UmiAppearanceTargetSizeCertificationArchiveRead(UmiArchiveReader *reader, UmiAppearanceTargetSizeCertification *value)
{
    UmiArchiveReadText(reader, value->target_id, sizeof(value->target_id));
    value->width_dp = UmiArchiveReadDouble(reader);
    value->height_dp = UmiArchiveReadDouble(reader);
    value->required_width_dp = UmiArchiveReadDouble(reader);
    value->required_height_dp = UmiArchiveReadDouble(reader);
    value->passed = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiAppearanceTargetSizeCertificationArchiveValidate(const UmiAppearanceTargetSizeCertification *value)
{
    return umi_appearance_target_size_certification_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_target_size_certification_archive_encode, umi_appearance_target_size_certification_archive_decode,
    UmiAppearanceTargetSizeCertification, UmiAppearanceTargetSizeCertificationArchiveSchema, UmiAppearanceTargetSizeCertificationArchiveBound, UmiAppearanceTargetSizeCertificationArchiveWrite, UmiAppearanceTargetSizeCertificationArchiveRead, UmiAppearanceTargetSizeCertificationArchiveValidate)
