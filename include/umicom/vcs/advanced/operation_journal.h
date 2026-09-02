/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/operation_journal.h
 *
 * PURPOSE:
 *   Record bounded VCS operation evidence for recovery, review and audit projections.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_ADVANCED_OPERATION_JOURNAL_H
#define UMICOM_VCS_ADVANCED_OPERATION_JOURNAL_H
#include "umicom/vcs/advanced/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the vcs advanced operation journal entry data shared with callers of this
 * public contract.
 */
typedef struct UmiVcsAdvancedOperationJournalEntry {
    UmiVcsAdvancedOperationKind kind; UmiVcsAdvancedState state; char reference[UMI_VCS_ADVANCED_TEXT_CAPACITY];
    uint64_t timestamp_seconds; UmiStatus status;
} UmiVcsAdvancedOperationJournalEntry;
/**
 * Represent the vcs advanced operation journal data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsAdvancedOperationJournal {
    uint32_t struct_size; uint32_t api_version;
    UmiVcsAdvancedOperationJournalEntry entries[UMI_VCS_ADVANCED_LIST_CAPACITY]; size_t count; uint64_t revision;
} UmiVcsAdvancedOperationJournal;
/**
 * Initialise vcs advanced operation journal from caller-provided values so later
 * operations receive a known state.
 */
void umi_vcs_advanced_operation_journal_init(UmiVcsAdvancedOperationJournal *journal);
/**
 * Add vcs advanced operation journal only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_vcs_advanced_operation_journal_append(UmiVcsAdvancedOperationJournal *journal,UmiVcsAdvancedOperationKind kind,
                                                     UmiVcsAdvancedState state,const char *reference,uint64_t timestamp,UmiStatus status);
/**
 * Provide the vcs advanced operation journal latest operation used by this module and its
 * client applications.
 */
const UmiVcsAdvancedOperationJournalEntry *umi_vcs_advanced_operation_journal_latest(const UmiVcsAdvancedOperationJournal *journal);
#ifdef __cplusplus
}
#endif
#endif
