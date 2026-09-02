/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/console_log.h
 *
 * PURPOSE:
 *   Publish the public console log contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONSOLE_LOG_H
#define UMICOM_FRONTEND_CONSOLE_LOG_H
#include "umicom/frontend/dev_types.h"
/**
 * Represent the frontend console entry data shared with callers of this public contract.
 */
typedef struct UmiFrontendConsoleEntry { uint64_t sequence; UmiFrontendConsoleLevel level; char source[UMI_FRONTEND_DEV_PATH_CAPACITY]; char message[UMI_FRONTEND_DEV_TEXT_CAPACITY]; uint32_t line; uint32_t column; } UmiFrontendConsoleEntry;
/**
 * Represent the frontend console log data shared with callers of this public contract.
 */
typedef struct UmiFrontendConsoleLog { UmiFrontendConsoleEntry items[UMI_FRONTEND_DEV_MAX_CONSOLE_ENTRIES]; size_t count; uint64_t next_sequence; } UmiFrontendConsoleLog;
/**
 * Initialise frontend console log from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_frontend_console_log_init(UmiFrontendConsoleLog *log);
/**
 * Add frontend console log only after its inputs and available capacity have been checked.
 */
UmiStatus umi_frontend_console_log_append(UmiFrontendConsoleLog *log,const UmiFrontendConsoleEntry *entry);
/**
 * Provide the frontend console log count level operation used by this module and its
 * client applications.
 */
size_t umi_frontend_console_log_count_level(const UmiFrontendConsoleLog *log,UmiFrontendConsoleLevel minimum);
/**
 * Release or reset state held by frontend console log so the same storage can be reused
 * safely.
 */
void umi_frontend_console_log_clear(UmiFrontendConsoleLog *log);
#endif
