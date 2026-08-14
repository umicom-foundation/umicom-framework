/* Umicom Framework | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/frontend/dev_types.h"
#include <string.h>
UmiStatus umi_frontend_dev_copy_text(char *d,size_t c,const char *s){size_t n;if(d==NULL||c==0U||s==NULL)return UMI_STATUS_INVALID_ARGUMENT;n=strlen(s);if(n>=c)return UMI_STATUS_CAPACITY_EXCEEDED;(void)memcpy(d,s,n+1U);return UMI_STATUS_OK;}
const char *umi_frontend_device_kind_text(UmiFrontendDeviceKind k){switch(k){case UMI_FRONTEND_DEVICE_DESKTOP:return "desktop";case UMI_FRONTEND_DEVICE_TABLET:return "tablet";case UMI_FRONTEND_DEVICE_PHONE:return "phone";case UMI_FRONTEND_DEVICE_CUSTOM:return "custom";default:return "unknown";}}
const char *umi_frontend_console_level_text(UmiFrontendConsoleLevel l){switch(l){case UMI_FRONTEND_CONSOLE_DEBUG:return "debug";case UMI_FRONTEND_CONSOLE_INFO:return "info";case UMI_FRONTEND_CONSOLE_WARNING:return "warning";case UMI_FRONTEND_CONSOLE_ERROR:return "error";default:return "unknown";}}
