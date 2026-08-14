/* Umicom Framework | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_FRONTEND_CONSOLE_LOG_H
#define UMICOM_FRONTEND_CONSOLE_LOG_H
#include "umicom/frontend/dev_types.h"
typedef struct UmiFrontendConsoleEntry { uint64_t sequence; UmiFrontendConsoleLevel level; char source[UMI_FRONTEND_DEV_PATH_CAPACITY]; char message[UMI_FRONTEND_DEV_TEXT_CAPACITY]; uint32_t line; uint32_t column; } UmiFrontendConsoleEntry;
typedef struct UmiFrontendConsoleLog { UmiFrontendConsoleEntry items[UMI_FRONTEND_DEV_MAX_CONSOLE_ENTRIES]; size_t count; uint64_t next_sequence; } UmiFrontendConsoleLog;
UmiStatus umi_frontend_console_log_init(UmiFrontendConsoleLog *log);
UmiStatus umi_frontend_console_log_append(UmiFrontendConsoleLog *log,const UmiFrontendConsoleEntry *entry);
size_t umi_frontend_console_log_count_level(const UmiFrontendConsoleLog *log,UmiFrontendConsoleLevel minimum);
void umi_frontend_console_log_clear(UmiFrontendConsoleLog *log);
#endif
