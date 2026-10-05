/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/page_template.c
 *
 * PURPOSE:
 *   Describe reusable page templates without embedding application-specific logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/page_template.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer page template from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_page_template_init(UmiRadPageTemplate *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->template_id, sizeof item->template_id, "page_template");
    (void)umi_rad_copy_text(item->name, sizeof item->name, "page_template");
    (void)umi_rad_copy_text(item->shell_kind, sizeof item->shell_kind, "page_template");
    return UMI_STATUS_OK;
}
/* Check that visual designer page template satisfies its contract before another service relies on it. */
int umi_rad_page_template_is_valid(const UmiRadPageTemplate *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->template_id, '\0', sizeof(item->template_id)) == NULL) return 0;
    if (memchr(item->name, '\0', sizeof(item->name)) == NULL) return 0;
    if (memchr(item->shell_kind, '\0', sizeof(item->shell_kind)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->template_id) && item->name[0] != '\0';}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadPageTemplateArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x62c2bc27412debd4);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadPageTemplate *)0)->template_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadPageTemplate *)0)->name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadPageTemplate *)0)->shell_kind)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadPageTemplateArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadPageTemplate *)0)->template_id) - 1U +
        8U + sizeof(((UmiRadPageTemplate *)0)->name) - 1U +
        8U + sizeof(((UmiRadPageTemplate *)0)->shell_kind) - 1U +
        8U;
}
static void UmiRadPageTemplateArchiveWrite(UmiArchiveWriter *writer, const UmiRadPageTemplate *value)
{
    UmiArchiveWriteText(writer, value->template_id, sizeof(value->template_id));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteText(writer, value->shell_kind, sizeof(value->shell_kind));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->initial_components);
}
static void UmiRadPageTemplateArchiveRead(UmiArchiveReader *reader, UmiRadPageTemplate *value)
{
    UmiArchiveReadText(reader, value->template_id, sizeof(value->template_id));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    UmiArchiveReadText(reader, value->shell_kind, sizeof(value->shell_kind));
    value->initial_components = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
}
static UmiStatus UmiRadPageTemplateArchiveValidate(const UmiRadPageTemplate *value)
{
    return umi_rad_page_template_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_page_template_archive_encode, umi_rad_page_template_archive_decode,
    UmiRadPageTemplate, UmiRadPageTemplateArchiveSchema, UmiRadPageTemplateArchiveBound, UmiRadPageTemplateArchiveWrite, UmiRadPageTemplateArchiveRead, UmiRadPageTemplateArchiveValidate)
