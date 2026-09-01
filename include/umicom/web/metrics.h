/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/metrics.h
 *
 * PURPOSE:
 *   Record basic HTTP request and response counters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module has one narrow responsibility. Keeping the pieces separate makes the web platform easier to test and lets Studio, Trader and TMS reuse the same implementation.
 */

#ifndef UMICOM_WEB_METRICS_H
#define UMICOM_WEB_METRICS_H
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWebMetrics { uint64_t requests; uint64_t responses_2xx; uint64_t responses_4xx; uint64_t responses_5xx; uint64_t bytes_in; uint64_t bytes_out; } UmiWebMetrics;
void umi_web_metrics_init(UmiWebMetrics *metrics);
void umi_web_metrics_record(UmiWebMetrics *metrics,size_t bytes_in,int status,size_t bytes_out);
#ifdef __cplusplus
}
#endif
#endif
