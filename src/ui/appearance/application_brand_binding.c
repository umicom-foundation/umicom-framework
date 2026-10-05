/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/application_brand_binding.c
 *
 * PURPOSE:
 *   Bind a thin application identity to Framework-owned brand and theme-pack identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/application_brand_binding.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_application_brand_binding_init(UmiAppearanceApplicationBrandBinding *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->application_id,sizeof item->application_id,"studio");
    (void)umi_appearance_copy_text(item->brand_id,sizeof item->brand_id,"studio.brand");
    (void)umi_appearance_copy_text(item->theme_pack_id,sizeof item->theme_pack_id,"studio.dark");
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_application_brand_binding_is_valid(const UmiAppearanceApplicationBrandBinding *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->application_id, '\0', sizeof(item->application_id)) == NULL) return 0;
    if (memchr(item->brand_id, '\0', sizeof(item->brand_id)) == NULL) return 0;
    if (memchr(item->theme_pack_id, '\0', sizeof(item->theme_pack_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->application_id) && umi_appearance_id_valid(item->brand_id) && umi_appearance_id_valid(item->theme_pack_id));
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceApplicationBrandBindingArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x1a5bdd1406b209cf);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceApplicationBrandBinding *)0)->application_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceApplicationBrandBinding *)0)->brand_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceApplicationBrandBinding *)0)->theme_pack_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceApplicationBrandBindingArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceApplicationBrandBinding *)0)->application_id) - 1U +
        8U + sizeof(((UmiAppearanceApplicationBrandBinding *)0)->brand_id) - 1U +
        8U + sizeof(((UmiAppearanceApplicationBrandBinding *)0)->theme_pack_id) - 1U;
}
static void UmiAppearanceApplicationBrandBindingArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceApplicationBrandBinding *value)
{
    UmiArchiveWriteText(writer, value->application_id, sizeof(value->application_id));
    UmiArchiveWriteText(writer, value->brand_id, sizeof(value->brand_id));
    UmiArchiveWriteText(writer, value->theme_pack_id, sizeof(value->theme_pack_id));
}
static void UmiAppearanceApplicationBrandBindingArchiveRead(UmiArchiveReader *reader, UmiAppearanceApplicationBrandBinding *value)
{
    UmiArchiveReadText(reader, value->application_id, sizeof(value->application_id));
    UmiArchiveReadText(reader, value->brand_id, sizeof(value->brand_id));
    UmiArchiveReadText(reader, value->theme_pack_id, sizeof(value->theme_pack_id));
}
static UmiStatus UmiAppearanceApplicationBrandBindingArchiveValidate(const UmiAppearanceApplicationBrandBinding *value)
{
    return umi_appearance_application_brand_binding_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_application_brand_binding_archive_encode, umi_appearance_application_brand_binding_archive_decode,
    UmiAppearanceApplicationBrandBinding, UmiAppearanceApplicationBrandBindingArchiveSchema, UmiAppearanceApplicationBrandBindingArchiveBound, UmiAppearanceApplicationBrandBindingArchiveWrite, UmiAppearanceApplicationBrandBindingArchiveRead, UmiAppearanceApplicationBrandBindingArchiveValidate)
