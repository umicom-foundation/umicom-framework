/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/event.h
 *
 * PURPOSE:
 *   Define bounded security-event evidence for authentication, authorisation,
 *   trust and privileged tool activity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SECURITY_EVENT_H
#define UMICOM_SECURITY_EVENT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_SECURITY_EVENT_TEXT_CAPACITY 192U
#define UMI_SECURITY_EVENT_MAX 2048U
typedef struct UmiSecurityEvent {
    uint64_t sequence;
    uint64_t timestamp_ns;
    uint64_t correlation_id;
    char principal[128];
    char action[UMI_SECURITY_EVENT_TEXT_CAPACITY];
    char resource[UMI_SECURITY_EVENT_TEXT_CAPACITY];
    int allowed;
} UmiSecurityEvent;
typedef struct UmiSecurityEventLog UmiSecurityEventLog;
UmiStatus umi_security_event_log_create(UmiSecurityEventLog **out_log);
void umi_security_event_log_destroy(UmiSecurityEventLog *log);
UmiStatus umi_security_event_log_append(UmiSecurityEventLog *log,
                                        const UmiSecurityEvent *event);
size_t umi_security_event_log_count(const UmiSecurityEventLog *log);
UmiStatus umi_security_event_log_at(const UmiSecurityEventLog *log,
                                    size_t index,
                                    UmiSecurityEvent *out_event);
size_t umi_security_event_log_denied_count(const UmiSecurityEventLog *log);
#ifdef __cplusplus
}
#endif

#endif
