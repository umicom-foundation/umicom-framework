/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/remote_path_mapping.h
 *
 * PURPOSE:
 *   Translate local/remote path prefixes without leaking path rules into Studio.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_PATH_MAPPING_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_PATH_MAPPING_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteRemotePathMapping { char local_prefix[UMI_TERMINAL_REMOTE_PATH_CAPACITY]; char remote_prefix[UMI_TERMINAL_REMOTE_PATH_CAPACITY]; } UmiTerminalRemoteRemotePathMapping;
void umi_terminal_remote_remote_path_mapping_init(UmiTerminalRemoteRemotePathMapping *value,const char *local_prefix,const char *remote_prefix);
UmiStatus umi_terminal_remote_remote_path_mapping_to_remote(const UmiTerminalRemoteRemotePathMapping *value,const char *local_path,char *out,size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
