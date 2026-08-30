/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/schema_fingerprint.c
 *
 * PURPOSE:
 *   Produce deterministic schema fingerprints for migration checkpoints, caches and release evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/schema_fingerprint.h"
#include <string.h>

/* Fingerprints avoid SQL text so semantically equivalent snapshots compare consistently. */
uint64_t umi_data_schema_fingerprint_compute(const UmiDataSchemaSnapshot *snapshot){uint64_t h=UINT64_C(1469598103934665603);size_t i,j;if(snapshot==NULL)return 0U;for(i=0;i<snapshot->count;++i){h=umi_data_enterprise_hash64(snapshot->items[i].table_id,strlen(snapshot->items[i].table_id),h);for(j=0;j<snapshot->items[i].column_count;++j){h=umi_data_enterprise_hash64(snapshot->items[i].columns[j].column_id,strlen(snapshot->items[i].columns[j].column_id),h);h=umi_data_enterprise_hash64(&snapshot->items[i].columns[j].kind,sizeof(snapshot->items[i].columns[j].kind),h);}}return h;}
