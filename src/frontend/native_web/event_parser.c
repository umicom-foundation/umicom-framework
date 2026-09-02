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
/* Provide the parse event kind operation used by this module and its client applications. */
static UmiNativeWebEventKind parse_event_kind(const char *s){/* Use the stable identifier comparison to choose the matching record or policy. */ if(strcmp(s,"click")==0)return UMI_NATIVE_WEB_EVENT_CLICK;/* Use the stable identifier comparison to choose the matching record or policy. */ if(strcmp(s,"input")==0)return UMI_NATIVE_WEB_EVENT_INPUT;/* Use the stable identifier comparison to choose the matching record or policy. */ if(strcmp(s,"change")==0)return UMI_NATIVE_WEB_EVENT_CHANGE;/* Use the stable identifier comparison to choose the matching record or policy. */ if(strcmp(s,"key")==0)return UMI_NATIVE_WEB_EVENT_KEY;/* Use the stable identifier comparison to choose the matching record or policy. */ if(strcmp(s,"focus")==0)return UMI_NATIVE_WEB_EVENT_FOCUS;/* Use the stable identifier comparison to choose the matching record or policy. */ if(strcmp(s,"blur")==0)return UMI_NATIVE_WEB_EVENT_BLUR;/* Use the stable identifier comparison to choose the matching record or policy. */ if(strcmp(s,"dragstart")==0)return UMI_NATIVE_WEB_EVENT_DRAG_START;/* Use the stable identifier comparison to choose the matching record or policy. */ if(strcmp(s,"dragover")==0)return UMI_NATIVE_WEB_EVENT_DRAG_OVER;/* Use the stable identifier comparison to choose the matching record or policy. */ if(strcmp(s,"drop")==0)return UMI_NATIVE_WEB_EVENT_DROP;/* Use the stable identifier comparison to choose the matching record or policy. */ if(strcmp(s,"command")==0)return UMI_NATIVE_WEB_EVENT_COMMAND;/* Use the stable identifier comparison to choose the matching record or policy. */ if(strcmp(s,"context")==0)return UMI_NATIVE_WEB_EVENT_CONTEXT;return 0;}
/*
 * Read native web event into validated module state and return a status when input cannot
 * be used.
 */
UmiStatus umi_native_web_event_parse(const char *message,uint64_t sequence,UmiNativeWebUiEvent *out_event){char copy[UMI_NATIVE_WEB_VALUE_CAPACITY*2U];char *parts[4]={0};char *p;size_t i=0U;UmiNativeWebEventKind kind;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(message==NULL||out_event==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strlen(message)>=sizeof(copy))return UMI_STATUS_CAPACITY_EXCEEDED;(void)strcpy(copy,message);parts[i++]=copy;/* Visit each bounded item once so every record receives the same rule. */ for(p=copy;*p!='\0'&&i<4U;++p)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(*p=='|'){*p='\0';parts[i++]=p+1;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i<2U)return UMI_STATUS_PARSE_ERROR;kind=parse_event_kind(parts[0]);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(kind==0)return UMI_STATUS_PARSE_ERROR;return umi_native_web_ui_event_init(out_event,kind,parts[1],i>2U?parts[2]:"",i>3U?parts[3]:"",sequence);}

