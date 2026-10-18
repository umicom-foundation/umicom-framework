/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/symbol.h
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
#ifndef UMICOM_COMPILER_NATIVE_SYMBOL_H
#define UMICOM_COMPILER_NATIVE_SYMBOL_H
#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named native symbol kind values accepted by this public contract.
 */
typedef enum UmiNativeSymbolKind { UMI_NC_SYMBOL_VARIABLE=1, UMI_NC_SYMBOL_FUNCTION=2, UMI_NC_SYMBOL_TYPE=3, UMI_NC_SYMBOL_ENUMERATOR=4, UMI_NC_SYMBOL_LABEL=5 } UmiNativeSymbolKind;
/**
 * Represent the native symbol data shared with callers of this public contract.
 */
typedef struct UmiNativeSymbol { uint32_t id; UmiNativeSymbolKind kind; char name[UMI_NC_NAME_CAPACITY]; uint32_t type_id; uint32_t scope_id; bool defined; bool external_linkage; bool internal_linkage; } UmiNativeSymbol;
/**
 * Initialise nc symbol from caller-provided values so later operations receive a known
 * state.
 */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_nc_symbol_init(UmiNativeSymbol *symbol,uint32_t id,UmiNativeSymbolKind kind,const char *name,uint32_t type_id,uint32_t scope_id);
/**
 * Check that nc symbol satisfies its contract before another service relies on it.
 */
UmiStatus umi_nc_symbol_validate(const UmiNativeSymbol *symbol);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_nc_symbol_archive_encode(const UmiNativeSymbol *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_nc_symbol_archive_decode(const void *bytes, size_t byte_count,
    UmiNativeSymbol *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiNativeSymbol object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_nc_symbol_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiNativeSymbol *workspace, UmiNativeSymbol *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_nc_symbol_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiNativeSymbol *expected, UmiNativeSymbol *value);


#ifdef __cplusplus
}
#endif
#endif
