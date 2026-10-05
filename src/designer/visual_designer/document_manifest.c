/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/document_manifest.c
 *
 * PURPOSE:
 *   Summarise the pages, forms, components and bindings in a visual document.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/document_manifest.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer document manifest from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_document_manifest_init(UmiRadDocumentManifest *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->application_id, sizeof item->application_id, "document_manifest");
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer document manifest satisfies its contract before another service relies on
 * it.
 */
int umi_rad_document_manifest_is_valid(const UmiRadDocumentManifest *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->application_id, '\0', sizeof(item->application_id)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->application_id);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadDocumentManifestArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xe953751960bd7791);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadDocumentManifest *)0)->application_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadDocumentManifestArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadDocumentManifest *)0)->application_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiRadDocumentManifestArchiveWrite(UmiArchiveWriter *writer, const UmiRadDocumentManifest *value)
{
    UmiArchiveWriteText(writer, value->application_id, sizeof(value->application_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->page_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->form_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->component_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->binding_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiRadDocumentManifestArchiveRead(UmiArchiveReader *reader, UmiRadDocumentManifest *value)
{
    UmiArchiveReadText(reader, value->application_id, sizeof(value->application_id));
    value->page_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->form_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->component_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->binding_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiRadDocumentManifestArchiveValidate(const UmiRadDocumentManifest *value)
{
    return umi_rad_document_manifest_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_document_manifest_archive_encode, umi_rad_document_manifest_archive_decode,
    UmiRadDocumentManifest, UmiRadDocumentManifestArchiveSchema, UmiRadDocumentManifestArchiveBound, UmiRadDocumentManifestArchiveWrite, UmiRadDocumentManifestArchiveRead, UmiRadDocumentManifestArchiveValidate)
