/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/sparse_index.h
 *
 * PURPOSE:
 *   Map sparse stable row keys to logical indexes for large data sets.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_SPARSE_INDEX_H
#define UMICOM_UI_ENTERPRISE_SPARSE_INDEX_H

#include "umicom/ui/enterprise/types.h"
typedef struct UmiUiEntSparseIndexEntry { uint64_t key; size_t logical_index; } UmiUiEntSparseIndexEntry;
typedef struct UmiUiEntSparseIndex { UmiUiEntSparseIndexEntry entries[UMI_UI_ENT_MAX_CACHE]; size_t count; } UmiUiEntSparseIndex;
void umi_ui_ent_sparse_index_init(UmiUiEntSparseIndex *index);
UmiStatus umi_ui_ent_sparse_index_put(UmiUiEntSparseIndex *index,uint64_t key,size_t logical_index);
int umi_ui_ent_sparse_index_find(const UmiUiEntSparseIndex *index,uint64_t key,size_t *out_logical_index);

#endif
