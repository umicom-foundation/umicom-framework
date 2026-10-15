/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/declaration.h
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
#ifndef UMICOM_COMPILER_NATIVE_DECLARATION_H
#define UMICOM_COMPILER_NATIVE_DECLARATION_H
#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named native declaration kind values accepted by this public contract.
 */
typedef enum UmiNativeDeclarationKind { UMI_NC_DECL_VARIABLE=1, UMI_NC_DECL_FUNCTION=2, UMI_NC_DECL_TYPEDEF=3, UMI_NC_DECL_STRUCT=4, UMI_NC_DECL_UNION=5, UMI_NC_DECL_ENUM=6 } UmiNativeDeclarationKind;
/**
 * Represent the native declaration data shared with callers of this public contract.
 */
typedef struct UmiNativeDeclaration { uint32_t node_id; UmiNativeDeclarationKind kind; char name[UMI_NC_NAME_CAPACITY]; uint32_t type_id; bool external_linkage; bool internal_linkage; bool definition; } UmiNativeDeclaration;
/**
 * Initialise nc declaration from caller-provided values so later operations receive a
 * known state.
 */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_nc_declaration_init(UmiNativeDeclaration *declaration,uint32_t node_id,UmiNativeDeclarationKind kind,const char *name,uint32_t type_id);
/**
 * Check that nc declaration satisfies its contract before another service relies on it.
 */
UmiStatus umi_nc_declaration_validate(const UmiNativeDeclaration *declaration);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_nc_declaration_archive_encode(const UmiNativeDeclaration *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_nc_declaration_archive_decode(const void *bytes, size_t byte_count,
    UmiNativeDeclaration *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_nc_declaration_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiNativeDeclaration *expected, UmiNativeDeclaration *value);


#ifdef __cplusplus
}
#endif
#endif
