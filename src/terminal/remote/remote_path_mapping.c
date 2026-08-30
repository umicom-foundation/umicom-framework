/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/remote_path_mapping.c
 *
 * PURPOSE:
 *   Implement prefix-safe local-to-remote path mapping.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/remote_path_mapping.h"
#include <stdio.h>
#include <string.h>
void umi_terminal_remote_remote_path_mapping_init(UmiTerminalRemoteRemotePathMapping *value,const char *local_prefix,const char *remote_prefix) { if(!value) return; value->local_prefix[0]='\0'; value->remote_prefix[0]='\0'; if(local_prefix) (void)umi_terminal_remote_copy_text(value->local_prefix,sizeof(value->local_prefix),local_prefix); if(remote_prefix) (void)umi_terminal_remote_copy_text(value->remote_prefix,sizeof(value->remote_prefix),remote_prefix); }
UmiStatus umi_terminal_remote_remote_path_mapping_to_remote(const UmiTerminalRemoteRemotePathMapping *value,const char *local_path,char *out,size_t capacity) { size_t n; int w; if(!value||!local_path||!out||capacity==0U) return UMI_STATUS_INVALID_ARGUMENT; n=strlen(value->local_prefix); if(n==0U||strncmp(local_path,value->local_prefix,n)!=0) return UMI_STATUS_NOT_FOUND; w=snprintf(out,capacity,"%s%s",value->remote_prefix,local_path+n); return w<0||(size_t)w>=capacity?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_OK; }
