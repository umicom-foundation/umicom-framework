/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/clipboard.c
 *
 * PURPOSE:
 *   Implement a thread-safe in-memory clipboard with revision tracking.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/clipboard.h"
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/threading.h"
#include "ui_internal.h"


struct UmiUiClipboard { char text[UMI_UI_CLIPBOARD_TEXT_CAPACITY]; uint64_t revision; UmiMutex *mutex; };
/*
 * Initialise ui clipboard from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ui_clipboard_create(UmiUiClipboard **out){UmiUiClipboard *c;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;c=calloc(1U,sizeof(*c));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL)return UMI_STATUS_OUT_OF_MEMORY;s=umi_mutex_create(&c->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK){free(c);return s;}c->revision=1U;*out=c;return UMI_STATUS_OK;}
/* Release or reset state held by ui clipboard so the same storage can be reused safely. */
void umi_ui_clipboard_destroy(UmiUiClipboard *c){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL)return;umi_mutex_destroy(c->mutex);free(c);}
/*
 * Provide the ui clipboard set text operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_clipboard_set_text(UmiUiClipboard *c,const char *text){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||text==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(c->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_ui_copy_text(c->text,sizeof(c->text),text)){(void)umi_mutex_unlock(c->mutex);return UMI_STATUS_CAPACITY_EXCEEDED;}c->revision=umi_ui_next_revision(c->revision);(void)umi_mutex_unlock(c->mutex);return UMI_STATUS_OK;}
/*
 * Provide the ui clipboard get text operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_clipboard_get_text(const UmiUiClipboard *c,char *out,size_t cap){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||out==NULL||cap==0U)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(c->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_ui_copy_text(out,cap,c->text)){(void)umi_mutex_unlock(c->mutex);return UMI_STATUS_CAPACITY_EXCEEDED;}(void)umi_mutex_unlock(c->mutex);return UMI_STATUS_OK;}
/* Release or reset state held by ui clipboard so the same storage can be reused safely. */
void umi_ui_clipboard_clear(UmiUiClipboard *c){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL)return;(void)umi_mutex_lock(c->mutex);c->text[0]='\0';c->revision=umi_ui_next_revision(c->revision);(void)umi_mutex_unlock(c->mutex);}
/*
 * Provide the ui clipboard revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_clipboard_revision(const UmiUiClipboard *c){uint64_t r=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c!=NULL){(void)umi_mutex_lock(c->mutex);r=c->revision;(void)umi_mutex_unlock(c->mutex);}return r;}
