/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/theme_scope.c
 *
 * PURPOSE:
 *   Describe the semantic scope at which a theme override is applied.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/theme_scope.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_theme_scope_init(UmiAppearanceThemeScope *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->scope_id,sizeof item->scope_id,"scope.system");
    (void)umi_appearance_copy_text(item->owner_id,sizeof item->owner_id,"system");
    item->scope=UMI_APPEARANCE_SCOPE_SYSTEM;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_theme_scope_is_valid(const UmiAppearanceThemeScope *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->scope_id, '\0', sizeof(item->scope_id)) == NULL) return 0;
    if (memchr(item->owner_id, '\0', sizeof(item->owner_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->scope_id) && umi_appearance_id_valid(item->owner_id));
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceThemeScopeArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xfd8bf447a118a610);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceThemeScope *)0)->scope_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceThemeScope *)0)->owner_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceThemeScopeArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceThemeScope *)0)->scope_id) - 1U +
        8U +
        8U + sizeof(((UmiAppearanceThemeScope *)0)->owner_id) - 1U;
}
static void UmiAppearanceThemeScopeArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceThemeScope *value)
{
    UmiArchiveWriteText(writer, value->scope_id, sizeof(value->scope_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->scope);
    UmiArchiveWriteText(writer, value->owner_id, sizeof(value->owner_id));
}
static void UmiAppearanceThemeScopeArchiveRead(UmiArchiveReader *reader, UmiAppearanceThemeScope *value)
{
    UmiArchiveReadText(reader, value->scope_id, sizeof(value->scope_id));
    value->scope = (UmiAppearanceScope)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    UmiArchiveReadText(reader, value->owner_id, sizeof(value->owner_id));
}
static UmiStatus UmiAppearanceThemeScopeArchiveValidate(const UmiAppearanceThemeScope *value)
{
    return umi_appearance_theme_scope_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_theme_scope_archive_encode, umi_appearance_theme_scope_archive_decode,
    UmiAppearanceThemeScope, UmiAppearanceThemeScopeArchiveSchema, UmiAppearanceThemeScopeArchiveBound, UmiAppearanceThemeScopeArchiveWrite, UmiAppearanceThemeScopeArchiveRead, UmiAppearanceThemeScopeArchiveValidate)
