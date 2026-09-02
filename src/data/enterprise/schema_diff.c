/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/schema_diff.c
 *
 * PURPOSE:
 *   Compare two schema snapshots and classify table additions, removals and structural changes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/schema_diff.h"
#include <string.h>

/* Diffing deliberately uses semantic IDs rather than backend-specific DDL text. */
UmiStatus umi_data_schema_diff_compute(const UmiDataSchemaSnapshot *before,const UmiDataSchemaSnapshot *after,UmiDataSchemaDiff *out_diff){size_t i,j;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(before==NULL||after==NULL||out_diff==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(out_diff,0,sizeof(*out_diff));/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<before->count;++i){bool found=false;/* Visit each bounded item once so every record receives the same rule. */ for(j=0;j<after->count;++j)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(before->items[i].table_id,after->items[j].table_id)==0){found=true;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(before->items[i].column_count==after->items[j].column_count)out_diff->unchanged_tables++;/* Use this fallback path when the earlier condition does not apply. */ else out_diff->changed_tables++;break;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!found)out_diff->removed_tables++;}/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<after->count;++i){bool found=false;/* Visit each bounded item once so every record receives the same rule. */ for(j=0;j<before->count;++j)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(after->items[i].table_id,before->items[j].table_id)==0){found=true;break;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!found)out_diff->added_tables++;}out_diff->compatibility=out_diff->removed_tables>0U?UMI_DATA_BREAKING:(out_diff->changed_tables>0U?UMI_DATA_COMPATIBLE_WITH_REBUILD:UMI_DATA_COMPATIBLE);return UMI_STATUS_OK;}
