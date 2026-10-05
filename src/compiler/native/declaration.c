/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/declaration.c
 *
 * PURPOSE:
 *   Describe top-level and local C declarations independently from parser implementation details.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/declaration.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise nc declaration from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_nc_declaration_init(UmiNativeDeclaration *d,uint32_t node,UmiNativeDeclarationKind kind,const char *name,uint32_t type){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||node==0U||name==NULL||name[0]=='\0'||type==0U)return UMI_STATUS_INVALID_ARGUMENT;memset(d,0,sizeof(*d));d->node_id=node;d->kind=kind;d->type_id=type;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_copy_text(d->name,sizeof(d->name),name)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;return UMI_STATUS_OK; }
/* Check that nc declaration satisfies its contract before another service relies on it. */
UmiStatus umi_nc_declaration_validate(const UmiNativeDeclaration *d){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (d == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(d->name, '\0', sizeof(d->name)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
 /* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||d->node_id==0U||d->name[0]=='\0'||d->type_id==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d->external_linkage&&d->internal_linkage)return UMI_STATUS_INVALID_STATE;return UMI_STATUS_OK; }

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiNativeDeclarationArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xa8b28a8131e3ed95);
    schema = (schema ^ (uint64_t)sizeof(((UmiNativeDeclaration *)0)->name)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiNativeDeclarationArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U + sizeof(((UmiNativeDeclaration *)0)->name) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiNativeDeclarationArchiveWrite(UmiArchiveWriter *writer, const UmiNativeDeclaration *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->node_id);
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->type_id);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->external_linkage);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->internal_linkage);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->definition);
}
static void UmiNativeDeclarationArchiveRead(UmiArchiveReader *reader, UmiNativeDeclaration *value)
{
    value->node_id = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->kind = (UmiNativeDeclarationKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    value->type_id = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->external_linkage = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->internal_linkage = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->definition = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiNativeDeclarationArchiveValidate(const UmiNativeDeclaration *value)
{
    return umi_nc_declaration_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_nc_declaration_archive_encode, umi_nc_declaration_archive_decode,
    UmiNativeDeclaration, UmiNativeDeclarationArchiveSchema, UmiNativeDeclarationArchiveBound, UmiNativeDeclarationArchiveWrite, UmiNativeDeclarationArchiveRead, UmiNativeDeclarationArchiveValidate)
