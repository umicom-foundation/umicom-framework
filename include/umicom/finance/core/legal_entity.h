/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/legal_entity.h
 *
 * PURPOSE:
 *   Define legal entities shared by all financial applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_LEGAL_ENTITY_H
#define UMICOM_FINANCE_CORE_LEGAL_ENTITY_H

#include "umicom/finance/core/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the legal entity data shared with callers of this public contract.
 */
typedef struct UmiLegalEntity { UmiFinancialId entity_id; char name[UMI_FINANCIAL_CORE_NAME_CAPACITY]; char code[UMI_FINANCIAL_CORE_CODE_CAPACITY]; bool active; } UmiLegalEntity;
/* Initialize the typed financial record. */ UmiStatus umi_legal_entity_init(UmiLegalEntity *item,const char *id,const char *name,const char *code);
/* Validate the typed financial record. */ bool umi_legal_entity_is_valid(const UmiLegalEntity *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_legal_entity_archive_encode(const UmiLegalEntity *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_legal_entity_archive_decode(const void *bytes, size_t byte_count,
    UmiLegalEntity *value);

#ifdef __cplusplus
}
#endif

#endif
