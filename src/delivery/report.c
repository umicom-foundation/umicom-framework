/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/report.c
 *
 * PURPOSE:
 *   Summarise delivery evidence into a compact structure and human-readable text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Reports let Studio display the same release evidence that command-line tools and CI consume.
 */

#include "umicom/delivery/report.h"
#include <stdio.h>
#include <string.h>

void umi_delivery_report_init(UmiDeliveryReport *report)
{
    if (report != NULL) (void)memset(report, 0, sizeof(*report));
}
UmiStatus umi_delivery_report_format(const UmiDeliveryReport *report,
                                     char *buffer,
                                     size_t capacity)
{
    int written;
    if (report == NULL || buffer == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(buffer, capacity,
                       "artifacts=%zu gates=%zu failed=%zu packages=%zu tests=%u/%u ready=%s",
                       report->artifacts,
                       report->gates,
                       report->failed_gates,
                       report->packages,
                       report->tests_passed,
                       report->tests_passed + report->tests_failed,
                       report->ready ? "yes" : "no");
    return written >= 0 && (size_t)written < capacity
               ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}
