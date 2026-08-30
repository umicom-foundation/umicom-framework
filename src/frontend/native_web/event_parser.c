/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/event_parser.c
 *
 * PURPOSE:
 *   Parse the compact browser event protocol into validated semantic UI events.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/event_parser.h"

#include <string.h>
static UmiNativeWebEventKind parse_event_kind(const char *s){if(strcmp(s,"click")==0)return UMI_NATIVE_WEB_EVENT_CLICK;if(strcmp(s,"input")==0)return UMI_NATIVE_WEB_EVENT_INPUT;if(strcmp(s,"change")==0)return UMI_NATIVE_WEB_EVENT_CHANGE;if(strcmp(s,"key")==0)return UMI_NATIVE_WEB_EVENT_KEY;if(strcmp(s,"focus")==0)return UMI_NATIVE_WEB_EVENT_FOCUS;if(strcmp(s,"blur")==0)return UMI_NATIVE_WEB_EVENT_BLUR;if(strcmp(s,"dragstart")==0)return UMI_NATIVE_WEB_EVENT_DRAG_START;if(strcmp(s,"dragover")==0)return UMI_NATIVE_WEB_EVENT_DRAG_OVER;if(strcmp(s,"drop")==0)return UMI_NATIVE_WEB_EVENT_DROP;if(strcmp(s,"command")==0)return UMI_NATIVE_WEB_EVENT_COMMAND;if(strcmp(s,"context")==0)return UMI_NATIVE_WEB_EVENT_CONTEXT;return 0;}
UmiStatus umi_native_web_event_parse(const char *message,uint64_t sequence,UmiNativeWebUiEvent *out_event){char copy[UMI_NATIVE_WEB_VALUE_CAPACITY*2U];char *parts[4]={0};char *p;size_t i=0U;UmiNativeWebEventKind kind;if(message==NULL||out_event==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(strlen(message)>=sizeof(copy))return UMI_STATUS_CAPACITY_EXCEEDED;(void)strcpy(copy,message);parts[i++]=copy;for(p=copy;*p!='\0'&&i<4U;++p)if(*p=='|'){*p='\0';parts[i++]=p+1;}if(i<2U)return UMI_STATUS_PARSE_ERROR;kind=parse_event_kind(parts[0]);if(kind==0)return UMI_STATUS_PARSE_ERROR;return umi_native_web_ui_event_init(out_event,kind,parts[1],i>2U?parts[2]:"",i>3U?parts[3]:"",sequence);}

