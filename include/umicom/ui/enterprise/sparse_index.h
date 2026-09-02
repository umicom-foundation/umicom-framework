/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/sparse_index.h
 *
 * PURPOSE:
 *   Map sparse stable row keys to logical indexes for large data sets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_SPARSE_INDEX_H
#define UMICOM_UI_ENTERPRISE_SPARSE_INDEX_H

#include "umicom/ui/enterprise/types.h"
/**
 * Represent the ui ent sparse index entry data shared with callers of this public
 * contract.
 */
typedef struct UmiUiEntSparseIndexEntry { uint64_t key; size_t logical_index; } UmiUiEntSparseIndexEntry;
/**
 * Represent the ui ent sparse index data shared with callers of this public contract.
 */
typedef struct UmiUiEntSparseIndex { UmiUiEntSparseIndexEntry entries[UMI_UI_ENT_MAX_CACHE]; size_t count; } UmiUiEntSparseIndex;
/**
 * Initialise ui ent sparse index from caller-provided values so later operations receive a
 * known state.
 */
void umi_ui_ent_sparse_index_init(UmiUiEntSparseIndex *index);
/**
 * Provide the ui ent sparse index put operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_sparse_index_put(UmiUiEntSparseIndex *index,uint64_t key,size_t logical_index);
/**
 * Find ui ent sparse index while leaving the underlying catalogue or model owned by this
 * module.
 */
int umi_ui_ent_sparse_index_find(const UmiUiEntSparseIndex *index,uint64_t key,size_t *out_logical_index);

#endif
