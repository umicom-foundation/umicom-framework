/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/report.h
 *
 * PURPOSE:
 *   Summarise delivery evidence into a compact structure and human-readable text.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Reports let Studio display the same release evidence that command-line tools and CI consume.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_REPORT_H
#define INCLUDE_UMICOM_DELIVERY_REPORT_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeliveryReport {
    size_t artifacts;
    size_t gates;
    size_t failed_gates;
    size_t packages;
    uint32_t tests_passed;
    uint32_t tests_failed;
    int ready;
} UmiDeliveryReport;

void umi_delivery_report_init(UmiDeliveryReport *report);
UmiStatus umi_delivery_report_format(const UmiDeliveryReport *report,
                                     char *buffer,
                                     size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
