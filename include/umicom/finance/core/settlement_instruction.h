/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/settlement_instruction.h
 *
 * PURPOSE:
 *   Describe provider-neutral settlement instructions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_SETTLEMENT_INSTRUCTION_H
#define UMICOM_FINANCE_CORE_SETTLEMENT_INSTRUCTION_H

#include "umicom/finance/core/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the settlement instruction data shared with callers of this public contract.
 */
typedef struct UmiSettlementInstruction { UmiFinancialId instruction_id; UmiFinancialId parent_id; char name[UMI_FINANCIAL_CORE_NAME_CAPACITY]; UmiFinancialDate effective_date; uint32_t state; bool active; } UmiSettlementInstruction;
/* Initialize the typed financial record. */ UmiStatus umi_settlement_instruction_init(UmiSettlementInstruction *item,const char *id,const char *name,const char *parent_id,UmiFinancialDate effective_date,uint32_t state);
/* Validate the typed financial record. */ bool umi_settlement_instruction_is_valid(const UmiSettlementInstruction *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_settlement_instruction_archive_encode(const UmiSettlementInstruction *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_settlement_instruction_archive_decode(const void *bytes, size_t byte_count,
    UmiSettlementInstruction *value);

#ifdef __cplusplus
}
#endif

#endif
