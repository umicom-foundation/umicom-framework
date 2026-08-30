/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/layout_persistence_v2.c
 *
 * PURPOSE:
 *   Implement the layout persistence v2 behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Layout persistence boundary v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/ui/layout_persistence_v2.h"
#include <stdio.h>
#include <string.h>

static bool safe_field(const char *text) { return text != NULL && strchr(text,'\t') == NULL && strchr(text,'\n') == NULL && strchr(text,'\r') == NULL; }
static UmiStatus append_text(char *out_text,size_t capacity,size_t *used,const char *text)
{ size_t length = strlen(text); if (*used + length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED; (void)memcpy(out_text + *used,text,length + 1U); *used += length; return UMI_STATUS_OK; }

UmiStatus umi_ui_layout_persistence_v2_encode(const UmiUiLayoutPersistenceRecordV2 *record,char *out_text,size_t capacity)
{
    char line[1024U]; size_t used = 0U; size_t index; int length; UmiStatus status;
    if (record == NULL || out_text == NULL || capacity == 0U || record->schema_version != 2U || !safe_field(record->layout.layout_id) || !safe_field(record->layout.name)) return UMI_STATUS_INVALID_ARGUMENT;
    out_text[0] = '\0'; length = snprintf(line,sizeof(line),"UMILAYOUT2\t%u\t%llu\t%s\t%s\t%d\t%zu\t%llu\n",record->schema_version,(unsigned long long)record->saved_at_ns,record->layout.layout_id,record->layout.name,record->layout.locked ? 1 : 0,record->layout.window_count,(unsigned long long)record->layout.revision); if (length < 0 || (size_t)length >= sizeof(line)) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = append_text(out_text,capacity,&used,line); if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < record->layout.window_count; ++index) {
        const UmiUiWorkspaceWindow *window = &record->layout.windows[index]; const char *group = window->group_id[0] == '\0' ? "-" : window->group_id;
        if (!safe_field(window->window_id) || !safe_field(window->title) || !safe_field(window->tool_id) || !safe_field(group)) return UMI_STATUS_INVALID_ARGUMENT;
        length = snprintf(line,sizeof(line),"W\t%s\t%s\t%s\t%s\t%.8f\t%.8f\t%.8f\t%.8f\t%d\t%d\t%d\t%d\t%d\n",window->window_id,window->title,window->tool_id,group,window->x,window->y,window->width,window->height,window->visible ? 1 : 0,window->floating ? 1 : 0,window->maximised ? 1 : 0,window->closable ? 1 : 0,window->z_order); if (length < 0 || (size_t)length >= sizeof(line)) return UMI_STATUS_CAPACITY_EXCEEDED;
        status = append_text(out_text,capacity,&used,line); if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_layout_persistence_v2_decode(const char *text,UmiUiLayoutPersistenceRecordV2 *out_record)
{
    char buffer[UMI_UI_LAYOUT_ENCODED_CAPACITY]; char *line; size_t expected; size_t parsed = 0U; unsigned schema; unsigned long long saved; unsigned long long revision; int locked; int matched;
    if (text == NULL || out_record == NULL || strlen(text) >= sizeof(buffer)) return UMI_STATUS_INVALID_ARGUMENT;
    (void)snprintf(buffer,sizeof(buffer),"%s",text); (void)memset(out_record,0,sizeof(*out_record)); line = buffer;
    {
        char *end = strchr(line,'\n'); if (end == NULL) return UMI_STATUS_PARSE_ERROR; *end = '\0';
        matched = sscanf(line,"UMILAYOUT2\t%u\t%llu\t%127[^\t]\t%191[^\t]\t%d\t%zu\t%llu",&schema,&saved,out_record->layout.layout_id,out_record->layout.name,&locked,&expected,&revision); if (matched != 7 || schema != 2U || expected > UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS) return UMI_STATUS_PARSE_ERROR;
        out_record->schema_version = schema; out_record->saved_at_ns = (uint64_t)saved; out_record->layout.locked = locked != 0; out_record->layout.revision = (uint64_t)revision; line = end + 1;
    }
    while (*line != '\0' && parsed < expected) {
        UmiUiWorkspaceWindow *window = &out_record->layout.windows[parsed]; char group[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY]; int visible; int floating; int maximised; int closable; char *end = strchr(line,'\n'); bool had_newline = end != NULL; if (!had_newline) end = line + strlen(line); if (had_newline) *end = '\0';
        matched = sscanf(line,"W\t%127[^\t]\t%191[^\t]\t%127[^\t]\t%127[^\t]\t%lf\t%lf\t%lf\t%lf\t%d\t%d\t%d\t%d\t%d",window->window_id,window->title,window->tool_id,group,&window->x,&window->y,&window->width,&window->height,&visible,&floating,&maximised,&closable,&window->z_order); if (matched != 13) return UMI_STATUS_PARSE_ERROR;
        if (strcmp(group,"-") != 0) {
            (void)snprintf(window->group_id,sizeof(window->group_id),"%s",group);
        }
        window->visible = visible != 0;
        window->floating = floating != 0;
        window->maximised = maximised != 0;
        window->closable = closable != 0;
        parsed += 1U;
        line = had_newline ? end + 1 : end;
    }
    out_record->layout.window_count = parsed; return parsed == expected ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}
