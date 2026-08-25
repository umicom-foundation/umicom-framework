/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/integrity_scan.c
 *
 * PURPOSE:
 *   Aggregate integrity checks and classify database health without owning backend-specific verification commands.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/integrity_scan.h"
#include <string.h>

/* Begin establishes audit correlation before checks execute. */ UmiStatus umi_data_integrity_scan_begin(UmiDataIntegrityScan *scan,const char *scan_id,uint64_t started_at){UmiStatus s;if(scan==NULL||scan_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(scan,0,sizeof(*scan));s=umi_data_enterprise_copy_text(scan->scan_id,sizeof(scan->scan_id),scan_id);if(s!=UMI_STATUS_OK)return s;scan->started_at=started_at;scan->state=UMI_DATA_ENTERPRISE_UNKNOWN;return UMI_STATUS_OK;}
/* Failure evidence dominates warnings but both counters remain visible. */ UmiStatus umi_data_integrity_scan_record(UmiDataIntegrityScan *scan,bool warning,bool failure){if(scan==NULL||scan->scan_id[0]=='\0'||scan->finished_at!=0U)return UMI_STATUS_INVALID_STATE;scan->checks_run++;if(warning)scan->warnings++;if(failure)scan->failures++;return UMI_STATUS_OK;}
/* Finish derives state only after all caller-controlled checks have been recorded. */ UmiStatus umi_data_integrity_scan_finish(UmiDataIntegrityScan *scan,uint64_t finished_at){if(scan==NULL||scan->scan_id[0]=='\0'||finished_at<scan->started_at)return UMI_STATUS_INVALID_ARGUMENT;scan->finished_at=finished_at;scan->state=scan->failures>0U?UMI_DATA_ENTERPRISE_FAILED:(scan->warnings>0U?UMI_DATA_ENTERPRISE_DEGRADED:UMI_DATA_ENTERPRISE_READY);return UMI_STATUS_OK;}
