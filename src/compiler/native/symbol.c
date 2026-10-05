/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/symbol.c
 *
 * PURPOSE:
 *   Describe native compiler symbols, linkage and type ownership independently from storage implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/symbol.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise nc symbol from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_nc_symbol_init(UmiNativeSymbol *s,uint32_t id,UmiNativeSymbolKind kind,const char *name,uint32_t type,uint32_t scope){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||id==0U||name==NULL||name[0]=='\0'||scope==0U)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof(*s));s->id=id;s->kind=kind;s->type_id=type;s->scope_id=scope;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_copy_text(s->name,sizeof(s->name),name)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;return UMI_STATUS_OK;}
/* Check that nc symbol satisfies its contract before another service relies on it. */
UmiStatus umi_nc_symbol_validate(const UmiNativeSymbol *s){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (s == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(s->name, '\0', sizeof(s->name)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||s->id==0U||s->scope_id==0U||s->name[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->external_linkage&&s->internal_linkage)return UMI_STATUS_INVALID_STATE;return UMI_STATUS_OK;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiNativeSymbolArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x3818498a232b686b);
    schema = (schema ^ (uint64_t)sizeof(((UmiNativeSymbol *)0)->name)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiNativeSymbolArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U + sizeof(((UmiNativeSymbol *)0)->name) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiNativeSymbolArchiveWrite(UmiArchiveWriter *writer, const UmiNativeSymbol *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->id);
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->type_id);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->scope_id);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->defined);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->external_linkage);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->internal_linkage);
}
static void UmiNativeSymbolArchiveRead(UmiArchiveReader *reader, UmiNativeSymbol *value)
{
    value->id = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->kind = (UmiNativeSymbolKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    value->type_id = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->scope_id = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->defined = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->external_linkage = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->internal_linkage = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiNativeSymbolArchiveValidate(const UmiNativeSymbol *value)
{
    return umi_nc_symbol_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_nc_symbol_archive_encode, umi_nc_symbol_archive_decode,
    UmiNativeSymbol, UmiNativeSymbolArchiveSchema, UmiNativeSymbolArchiveBound, UmiNativeSymbolArchiveWrite, UmiNativeSymbolArchiveRead, UmiNativeSymbolArchiveValidate)
