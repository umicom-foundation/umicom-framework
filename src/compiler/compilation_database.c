/* Umicom Framework | Polyglot compilation database | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/compilation_database.h"
#include "umicom/compiler/language.h"
#include <stdio.h>
#include <string.h>

static UmiStatus json_append(char *output,size_t capacity,size_t *length,const char *text)
{ size_t count = strlen(text); if (*length + count + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED; (void)memcpy(output + *length,text,count + 1U); *length += count; return UMI_STATUS_OK; }
static UmiStatus json_string(char *output,size_t capacity,size_t *length,const char *text)
{
    const unsigned char *cursor; UmiStatus status = json_append(output,capacity,length,"\"");
    if (status != UMI_STATUS_OK) return status;
    for (cursor = (const unsigned char *)text; *cursor != 0U; ++cursor) {
        char encoded[8U] = {0};
        if (*cursor == '"' || *cursor == '\\') { encoded[0] = '\\'; encoded[1] = (char)*cursor; }
        else if (*cursor == '\n') { encoded[0] = '\\'; encoded[1] = 'n'; }
        else if (*cursor < 32U) (void)snprintf(encoded,sizeof(encoded),"\\u%04x",(unsigned int)*cursor);
        else encoded[0] = (char)*cursor;
        status = json_append(output,capacity,length,encoded); if (status != UMI_STATUS_OK) return status;
    }
    return json_append(output,capacity,length,"\"");
}
UmiStatus umi_polyglot_compilation_database_add(UmiPolyglotCompilationDatabase *database,const UmiPolyglotCompilationEntry *entry)
{
    size_t index;
    if (database == NULL || entry == NULL || entry->unit_id[0] == '\0' || entry->source[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < database->count; ++index) if (strcmp(database->items[index].source,entry->source) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (database->count >= UMI_POLYGLOT_DATABASE_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    database->items[database->count++] = *entry; database->revision += 1U; return UMI_STATUS_OK;
}
const UmiPolyglotCompilationEntry *umi_polyglot_compilation_database_find(const UmiPolyglotCompilationDatabase *database,const char *source)
{ size_t index; if (database == NULL || source == NULL) return NULL; for (index = 0U; index < database->count; ++index) if (strcmp(database->items[index].source,source) == 0) return &database->items[index]; return NULL; }
UmiStatus umi_polyglot_compilation_database_export_json(const UmiPolyglotCompilationDatabase *database,char *out_json,size_t capacity)
{
    size_t item,arg,length = 0U; UmiStatus status;
    if (database == NULL || out_json == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    out_json[0] = '\0'; status = json_append(out_json,capacity,&length,"[\n"); if (status != UMI_STATUS_OK) return status;
    for (item = 0U; item < database->count; ++item) {
        const UmiPolyglotCompilationEntry *entry = &database->items[item];
        status = json_append(out_json,capacity,&length,"  {\"directory\":"); if (status == UMI_STATUS_OK) status = json_string(out_json,capacity,&length,entry->directory);
        if (status == UMI_STATUS_OK) status = json_append(out_json,capacity,&length,",\"file\":");
        if (status == UMI_STATUS_OK) status = json_string(out_json,capacity,&length,entry->source);
        if (status == UMI_STATUS_OK) status = json_append(out_json,capacity,&length,",\"output\":");
        if (status == UMI_STATUS_OK) status = json_string(out_json,capacity,&length,entry->output);
        if (status == UMI_STATUS_OK) status = json_append(out_json,capacity,&length,",\"language\":");
        if (status == UMI_STATUS_OK) status = json_string(out_json,capacity,&length,umi_compiler_language_name(entry->language));
        if (status == UMI_STATUS_OK) status = json_append(out_json,capacity,&length,",\"arguments\":[");
        for (arg = 0U; status == UMI_STATUS_OK && arg < entry->plan.argument_count; ++arg) { if (arg > 0U) status = json_append(out_json,capacity,&length,","); if (status == UMI_STATUS_OK) status = json_string(out_json,capacity,&length,entry->plan.arguments[arg]); }
        if (status == UMI_STATUS_OK) status = json_append(out_json,capacity,&length,item + 1U < database->count ? "]},\n" : "]}\n");
        if (status != UMI_STATUS_OK) return status;
    }
    return json_append(out_json,capacity,&length,"]\n");
}
