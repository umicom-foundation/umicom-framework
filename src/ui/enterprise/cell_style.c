/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/cell_style.c
 *
 * PURPOSE:
 *   Describe semantic cell presentation independent of a renderer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/cell_style.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise ui ent cell style from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_ent_cell_style_init(UmiUiEntCellStyle *value){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->semantic_role[0]='\0';value->bold=0;value->italic=0;value->alignment=0;value->indent=0;value->alignment=0;return UMI_STATUS_OK;}
/* Check that ui ent cell style satisfies its contract before another service relies on it. */
int umi_ui_ent_cell_style_validate(const UmiUiEntCellStyle *value){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->semantic_role, '\0', sizeof(value->semantic_role)) == NULL) return 0;
return value!=NULL;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiEntCellStyleArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xeb297c6c160e31d2);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiEntCellStyle *)0)->semantic_role)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiEntCellStyleArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiEntCellStyle *)0)->semantic_role) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiUiEntCellStyleArchiveWrite(UmiArchiveWriter *writer, const UmiUiEntCellStyle *value)
{
    UmiArchiveWriteText(writer, value->semantic_role, sizeof(value->semantic_role));
    UmiArchiveWriteSigned(writer, (int64_t)value->bold);
    UmiArchiveWriteSigned(writer, (int64_t)value->italic);
    UmiArchiveWriteSigned(writer, (int64_t)value->alignment);
    UmiArchiveWriteSigned(writer, (int64_t)value->indent);
}
static void UmiUiEntCellStyleArchiveRead(UmiArchiveReader *reader, UmiUiEntCellStyle *value)
{
    UmiArchiveReadText(reader, value->semantic_role, sizeof(value->semantic_role));
    value->bold = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->italic = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->alignment = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->indent = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
}
static UmiStatus UmiUiEntCellStyleArchiveValidate(const UmiUiEntCellStyle *value)
{
    return umi_ui_ent_cell_style_validate(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_ent_cell_style_archive_encode, umi_ui_ent_cell_style_archive_decode,
    UmiUiEntCellStyle, UmiUiEntCellStyleArchiveSchema, UmiUiEntCellStyleArchiveBound, UmiUiEntCellStyleArchiveWrite, UmiUiEntCellStyleArchiveRead, UmiUiEntCellStyleArchiveValidate)
