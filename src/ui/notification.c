/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/notification.c
 *
 * PURPOSE:
 *   Implement chronological notifications with generated identifiers and dismissal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/notification.h"
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/threading.h"


struct UmiUiNotificationCentre { UmiUiNotification items[UMI_UI_NOTIFICATION_MAX]; size_t count; uint64_t next_id; UmiMutex *mutex; };
UmiStatus umi_ui_notification_centre_create(UmiUiNotificationCentre **out){UmiUiNotificationCentre *c;UmiStatus s;if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;c=calloc(1U,sizeof(*c));if(c==NULL)return UMI_STATUS_OUT_OF_MEMORY;s=umi_mutex_create(&c->mutex);if(s!=UMI_STATUS_OK){free(c);return s;}c->next_id=1U;*out=c;return UMI_STATUS_OK;}
void umi_ui_notification_centre_destroy(UmiUiNotificationCentre *c){if(c==NULL)return;umi_mutex_destroy(c->mutex);free(c);}
UmiStatus umi_ui_notification_publish(UmiUiNotificationCentre *c,const UmiUiNotification *n,uint64_t *out_id){UmiUiNotification stored;if(c==NULL||n==NULL||n->title[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(c->mutex);if(c->count>=UMI_UI_NOTIFICATION_MAX){(void)memmove(&c->items[0],&c->items[1],(UMI_UI_NOTIFICATION_MAX-1U)*sizeof(c->items[0]));c->count=UMI_UI_NOTIFICATION_MAX-1U;}stored=*n;stored.notification_id=c->next_id++;stored.dismissed=0;c->items[c->count++]=stored;if(out_id!=NULL)*out_id=stored.notification_id;(void)umi_mutex_unlock(c->mutex);return UMI_STATUS_OK;}
UmiStatus umi_ui_notification_dismiss(UmiUiNotificationCentre *c,uint64_t id){size_t i;if(c==NULL||id==0U)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(c->mutex);for(i=0U;i<c->count;++i)if(c->items[i].notification_id==id){c->items[i].dismissed=1;(void)umi_mutex_unlock(c->mutex);return UMI_STATUS_OK;}(void)umi_mutex_unlock(c->mutex);return UMI_STATUS_NOT_FOUND;}
UmiStatus umi_ui_notification_at(const UmiUiNotificationCentre *c,size_t index,UmiUiNotification *out){if(c==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(c->mutex);if(index>=c->count){(void)umi_mutex_unlock(c->mutex);return UMI_STATUS_NOT_FOUND;}*out=c->items[index];(void)umi_mutex_unlock(c->mutex);return UMI_STATUS_OK;}
size_t umi_ui_notification_count(const UmiUiNotificationCentre *c,int include){size_t i,n=0U;if(c==NULL)return 0U;(void)umi_mutex_lock(c->mutex);for(i=0U;i<c->count;++i)if(include||!c->items[i].dismissed)n++;(void)umi_mutex_unlock(c->mutex);return n;}
void umi_ui_notification_clear(UmiUiNotificationCentre *c){if(c==NULL)return;(void)umi_mutex_lock(c->mutex);c->count=0U;(void)umi_mutex_unlock(c->mutex);}
