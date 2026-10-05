/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/accessibility_model.c
 *
 * PURPOSE:
 *   Describe accessible row/column metadata for virtualised enterprise views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/accessibility_model.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise ui ent accessibility model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_ent_accessibility_model_init(UmiUiEntAccessibilityModel *value){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->role[0]='\0';value->name[0]='\0';value->description[0]='\0';value->row_index=0;value->column_index=0;value->set_size=0;value->position_in_set=0;value->position_in_set=1U;return UMI_STATUS_OK;}
/*
 * Check that ui ent accessibility model satisfies its contract before another service
 * relies on it.
 */
int umi_ui_ent_accessibility_model_validate(const UmiUiEntAccessibilityModel *value){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->role, '\0', sizeof(value->role)) == NULL) return 0;
    if (memchr(value->name, '\0', sizeof(value->name)) == NULL) return 0;
    if (memchr(value->description, '\0', sizeof(value->description)) == NULL) return 0;
return value!=NULL&&umi_ui_ent_id_valid(value->role)&&value->name[0]!='\0'&&value->position_in_set>0U;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiEntAccessibilityModelArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x14510118f17b86fa);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiEntAccessibilityModel *)0)->role)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiEntAccessibilityModel *)0)->name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiEntAccessibilityModel *)0)->description)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiEntAccessibilityModelArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiEntAccessibilityModel *)0)->role) - 1U +
        8U + sizeof(((UmiUiEntAccessibilityModel *)0)->name) - 1U +
        8U + sizeof(((UmiUiEntAccessibilityModel *)0)->description) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiUiEntAccessibilityModelArchiveWrite(UmiArchiveWriter *writer, const UmiUiEntAccessibilityModel *value)
{
    UmiArchiveWriteText(writer, value->role, sizeof(value->role));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteText(writer, value->description, sizeof(value->description));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->row_index);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->column_index);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->set_size);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->position_in_set);
}
static void UmiUiEntAccessibilityModelArchiveRead(UmiArchiveReader *reader, UmiUiEntAccessibilityModel *value)
{
    UmiArchiveReadText(reader, value->role, sizeof(value->role));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    UmiArchiveReadText(reader, value->description, sizeof(value->description));
    value->row_index = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->column_index = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->set_size = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->position_in_set = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
}
static UmiStatus UmiUiEntAccessibilityModelArchiveValidate(const UmiUiEntAccessibilityModel *value)
{
    return umi_ui_ent_accessibility_model_validate(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_ent_accessibility_model_archive_encode, umi_ui_ent_accessibility_model_archive_decode,
    UmiUiEntAccessibilityModel, UmiUiEntAccessibilityModelArchiveSchema, UmiUiEntAccessibilityModelArchiveBound, UmiUiEntAccessibilityModelArchiveWrite, UmiUiEntAccessibilityModelArchiveRead, UmiUiEntAccessibilityModelArchiveValidate)
