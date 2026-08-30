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
UmiStatus umi_ui_clipboard_create(UmiUiClipboard **out){UmiUiClipboard *c;UmiStatus s;if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;c=calloc(1U,sizeof(*c));if(c==NULL)return UMI_STATUS_OUT_OF_MEMORY;s=umi_mutex_create(&c->mutex);if(s!=UMI_STATUS_OK){free(c);return s;}c->revision=1U;*out=c;return UMI_STATUS_OK;}
void umi_ui_clipboard_destroy(UmiUiClipboard *c){if(c==NULL)return;umi_mutex_destroy(c->mutex);free(c);}
UmiStatus umi_ui_clipboard_set_text(UmiUiClipboard *c,const char *text){if(c==NULL||text==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(c->mutex);if(!umi_ui_copy_text(c->text,sizeof(c->text),text)){(void)umi_mutex_unlock(c->mutex);return UMI_STATUS_CAPACITY_EXCEEDED;}c->revision=umi_ui_next_revision(c->revision);(void)umi_mutex_unlock(c->mutex);return UMI_STATUS_OK;}
UmiStatus umi_ui_clipboard_get_text(const UmiUiClipboard *c,char *out,size_t cap){if(c==NULL||out==NULL||cap==0U)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(c->mutex);if(!umi_ui_copy_text(out,cap,c->text)){(void)umi_mutex_unlock(c->mutex);return UMI_STATUS_CAPACITY_EXCEEDED;}(void)umi_mutex_unlock(c->mutex);return UMI_STATUS_OK;}
void umi_ui_clipboard_clear(UmiUiClipboard *c){if(c==NULL)return;(void)umi_mutex_lock(c->mutex);c->text[0]='\0';c->revision=umi_ui_next_revision(c->revision);(void)umi_mutex_unlock(c->mutex);}
uint64_t umi_ui_clipboard_revision(const UmiUiClipboard *c){uint64_t r=0U;if(c!=NULL){(void)umi_mutex_lock(c->mutex);r=c->revision;(void)umi_mutex_unlock(c->mutex);}return r;}
