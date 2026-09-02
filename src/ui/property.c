/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/property.c
 *
 * PURPOSE:
 *   Implement a bounded, thread-safe and revisioned UI property bag.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/property.h"
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/threading.h"
#include "ui_internal.h"


typedef struct UmiUiPropertyEntry { char key[UMI_UI_PROPERTY_KEY_CAPACITY]; UmiUiValue value; } UmiUiPropertyEntry;
struct UmiUiPropertyBag { UmiUiPropertyEntry entries[UMI_UI_PROPERTY_MAX]; size_t count; uint64_t revision; UmiMutex *mutex; };
/* Find property while leaving the underlying catalogue or model owned by this module. */
static size_t property_find(const UmiUiPropertyBag *bag, const char *key)
{ size_t i; /* Visit each bounded item once so every record receives the same rule. */ for (i=0U;i<bag->count;++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(bag->entries[i].key,key)==0) return i; return SIZE_MAX; }
/*
 * Initialise ui property bag from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_property_bag_create(UmiUiPropertyBag **out_bag)
{ UmiUiPropertyBag *bag; UmiStatus s; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_bag==NULL)return UMI_STATUS_INVALID_ARGUMENT; *out_bag=NULL; bag=calloc(1U,sizeof(*bag)); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(bag==NULL)return UMI_STATUS_OUT_OF_MEMORY; s=umi_mutex_create(&bag->mutex); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK){free(bag);return s;} bag->revision=1U; *out_bag=bag; return UMI_STATUS_OK; }
/* Release or reset state held by ui property bag so the same storage can be reused safely. */
void umi_ui_property_bag_destroy(UmiUiPropertyBag *bag){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(bag==NULL)return;umi_mutex_destroy(bag->mutex);free(bag);}
/*
 * Copy ui property bag into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_ui_property_bag_set(UmiUiPropertyBag *bag,const char *key,const UmiUiValue *value)
{ size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(bag==NULL||key==NULL||value==NULL||key[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strlen(key)>=UMI_UI_PROPERTY_KEY_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED; (void)umi_mutex_lock(bag->mutex); i=property_find(bag,key); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(bag->count>=UMI_UI_PROPERTY_MAX){(void)umi_mutex_unlock(bag->mutex);return UMI_STATUS_CAPACITY_EXCEEDED;}i=bag->count++;(void)umi_ui_copy_text(bag->entries[i].key,sizeof(bag->entries[i].key),key);}bag->entries[i].value=*value;bag->revision=umi_ui_next_revision(bag->revision);(void)umi_mutex_unlock(bag->mutex);return UMI_STATUS_OK; }
/*
 * Provide the ui property bag get operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_property_bag_get(const UmiUiPropertyBag *bag,const char *key,UmiUiValue *out_value)
{ size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(bag==NULL||key==NULL||out_value==NULL)return UMI_STATUS_INVALID_ARGUMENT; (void)umi_mutex_lock(bag->mutex); i=property_find(bag,key); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){(void)umi_mutex_unlock(bag->mutex);return UMI_STATUS_NOT_FOUND;}*out_value=bag->entries[i].value;(void)umi_mutex_unlock(bag->mutex);return UMI_STATUS_OK; }
/*
 * Remove ui property bag while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_ui_property_bag_remove(UmiUiPropertyBag *bag,const char *key)
{ size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(bag==NULL||key==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(bag->mutex);i=property_find(bag,key);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){(void)umi_mutex_unlock(bag->mutex);return UMI_STATUS_NOT_FOUND;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i+1U<bag->count)(void)memmove(&bag->entries[i],&bag->entries[i+1U],(bag->count-i-1U)*sizeof(bag->entries[0]));bag->count--;bag->revision=umi_ui_next_revision(bag->revision);(void)umi_mutex_unlock(bag->mutex);return UMI_STATUS_OK; }
/* Release or reset state held by ui property bag so the same storage can be reused safely. */
void umi_ui_property_bag_clear(UmiUiPropertyBag *bag){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(bag==NULL)return;(void)umi_mutex_lock(bag->mutex);bag->count=0U;bag->revision=umi_ui_next_revision(bag->revision);(void)umi_mutex_unlock(bag->mutex);}
/*
 * Return the number of records represented by ui property bag without changing their
 * state.
 */
size_t umi_ui_property_bag_count(const UmiUiPropertyBag *bag){size_t n=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(bag!=NULL){(void)umi_mutex_lock(bag->mutex);n=bag->count;(void)umi_mutex_unlock(bag->mutex);}return n;}
/*
 * Provide the ui property bag revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_property_bag_revision(const UmiUiPropertyBag *bag){uint64_t r=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(bag!=NULL){(void)umi_mutex_lock(bag->mutex);r=bag->revision;(void)umi_mutex_unlock(bag->mutex);}return r;}
/*
 * Find ui property bag while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_property_bag_at(const UmiUiPropertyBag *bag,size_t index,UmiUiPropertySnapshot *out_property)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(bag==NULL||out_property==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(bag->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(index>=bag->count){(void)umi_mutex_unlock(bag->mutex);return UMI_STATUS_NOT_FOUND;}(void)umi_ui_copy_text(out_property->key,sizeof(out_property->key),bag->entries[index].key);out_property->value=bag->entries[index].value;(void)umi_mutex_unlock(bag->mutex);return UMI_STATUS_OK; }
