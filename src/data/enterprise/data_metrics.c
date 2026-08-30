/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/data_metrics.c
 *
 * PURPOSE:
 *   Capture bounded operational counters and derive useful rates for Data Server observability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/data_metrics.h"
#include <string.h>

/* Derived rates are recomputed rather than incrementally accumulated to avoid drift. */ UmiStatus umi_data_metrics_refresh(UmiDataMetrics *metrics){if(metrics==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(metrics->query_failures>metrics->queries||metrics->transaction_rollbacks>metrics->transactions)return UMI_STATUS_INVALID_STATE;metrics->query_success_percent=metrics->queries==0U?100.0:umi_data_enterprise_percent(metrics->queries-metrics->query_failures,metrics->queries);metrics->transaction_commit_percent=metrics->transactions==0U?100.0:umi_data_enterprise_percent(metrics->transactions-metrics->transaction_rollbacks,metrics->transactions);return UMI_STATUS_OK;}
