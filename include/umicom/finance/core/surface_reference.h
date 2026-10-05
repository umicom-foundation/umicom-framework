/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/surface_reference.h
 *
 * PURPOSE:
 *   Define provider-neutral surface references.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_SURFACE_REFERENCE_H
#define UMICOM_FINANCE_CORE_SURFACE_REFERENCE_H

#include "umicom/finance/core/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the surface reference data shared with callers of this public contract.
 */
typedef struct UmiSurfaceReference { UmiFinancialId reference_id; char name[UMI_FINANCIAL_CORE_NAME_CAPACITY]; char code[UMI_FINANCIAL_CORE_CODE_CAPACITY]; bool active; } UmiSurfaceReference;
/* Initialize the typed financial record. */ UmiStatus umi_surface_reference_init(UmiSurfaceReference *item,const char *id,const char *name,const char *code);
/* Validate the typed financial record. */ bool umi_surface_reference_is_valid(const UmiSurfaceReference *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_surface_reference_archive_encode(const UmiSurfaceReference *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_surface_reference_archive_decode(const void *bytes, size_t byte_count,
    UmiSurfaceReference *value);

#ifdef __cplusplus
}
#endif

#endif
