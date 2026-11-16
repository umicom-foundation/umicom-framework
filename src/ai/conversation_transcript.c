/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/conversation_transcript.c
 *
 * PURPOSE:
 *   Project reviewed conversation fields into literal Markdown after complete capacity and encoding admission.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/conversation_transcript.h"
#include "umicom/base/utf8.h"
#include <string.h>

/* The same small emitter counts and writes the exact representation. It owns no
 * storage and is private to this format; archive ownership and text validation
 * stay in their existing shared services. */
typedef struct TranscriptSink {
    char *output;
    size_t length;
    UmiStatus status;
} TranscriptSink;

static void transcript_append(TranscriptSink *sink,const char *text,size_t length)
{
    if(sink->status!=UMI_STATUS_OK) return;
    if(length>UMI_AI_TRANSCRIPT_MAX_BYTES-sink->length) {
        sink->status=UMI_STATUS_CAPACITY_EXCEEDED;
        return;
    }
    if(sink->output!=NULL) memcpy(sink->output+sink->length,text,length);
    sink->length+=length;
}

static void transcript_text(TranscriptSink *sink,const char *text)
{
    transcript_append(sink,text,strlen(text));
}

static void transcript_repeat(TranscriptSink *sink,char value,size_t count)
{
    if(sink->status!=UMI_STATUS_OK) return;
    if(count>UMI_AI_TRANSCRIPT_MAX_BYTES-sink->length) {
        sink->status=UMI_STATUS_CAPACITY_EXCEEDED;
        return;
    }
    if(sink->output!=NULL) memset(sink->output+sink->length,value,count);
    sink->length+=count;
}

static void transcript_number(TranscriptSink *sink,size_t number)
{
    /* Three decimal digits per byte cover every size_t value independently of
     * locale and printf length-modifier support. */
    char digits[3U*sizeof(size_t)+1U];
    size_t count=0U;
    do {
        digits[count++]=(char)('0'+number%10U);
        number/=10U;
    } while(number!=0U);
    while(count!=0U) transcript_append(sink,&digits[--count],1U);
}

static void transcript_literal(TranscriptSink *sink,const char *text)
{
    size_t run=0U,longest=0U;
    for(const char *at=text;*at!='\0';++at) {
        if(*at=='~') { ++run;if(run>longest) longest=run; }
        else run=0U;
    }
    size_t fence=longest<3U?3U:longest+1U;
    transcript_repeat(sink,'~',fence);transcript_text(sink,"text\n");
    transcript_text(sink,text);
    /* Always terminate the content line before a closing fence, including
     * empty text and a trailing CR. This separator is presentation syntax. */
    transcript_text(sink,"\n");transcript_repeat(sink,'~',fence);
    transcript_text(sink,"\n\n");
}

static int transcript_selected(const UmiAiConversationTranscriptOptions *options,
    const UmiAiMessage *message)
{
    return (options->roles & (1U << ((unsigned int)message->role-1U)))!=0U;
}

static UmiStatus transcript_field(const char *text)
{
    size_t length=strlen(text);
    if(!UmiUtf8Validate((const unsigned char *)text,length,NULL)) return UMI_STATUS_PARSE_ERROR;
    for(size_t i=0U;i<length;++i) {
        unsigned char value=(unsigned char)text[i];
        if((value<0x20U && value!='\t' && value!='\n' && value!='\r') || value==0x7fU)
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static UmiStatus transcript_validate(const UmiAiConversationRecord *record,
    const UmiAiConversationTranscriptOptions *options)
{
    if(options==NULL || UmiAiConversationRecordValidate(record)!=UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    if(options->first_message>record->conversation.message_count ||
        options->message_count>record->conversation.message_count-options->first_message ||
        options->roles==0U || (options->roles & ~UMI_AI_TRANSCRIPT_ALL)!=0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=transcript_field(record->title);
    for(size_t offset=0U;status==UMI_STATUS_OK && offset<options->message_count;++offset) {
        const UmiAiMessage *message=&record->conversation.messages[options->first_message+offset];
        if(!transcript_selected(options,message)) continue;
        if(options->include_message_names) status=transcript_field(message->name);
        if(status==UMI_STATUS_OK) status=transcript_field(message->text);
    }
    return status;
}

static void transcript_emit(const UmiAiConversationRecord *record,
    const UmiAiConversationTranscriptOptions *options,TranscriptSink *sink)
{
    transcript_text(sink,"## Conversation transcript\n\nClassification: ");
    transcript_text(sink,umi_ai_data_classification_text(record->classification));
    transcript_text(sink,"\n\n### Title\n\n");transcript_literal(sink,record->title);
    for(size_t offset=0U;offset<options->message_count;++offset) {
        size_t index=options->first_message+offset;
        const UmiAiMessage *message=&record->conversation.messages[index];
        if(!transcript_selected(options,message)) continue;
        transcript_text(sink,"### Message ");transcript_number(sink,index+1U);
        transcript_text(sink," - ");transcript_text(sink,umi_ai_role_text(message->role));
        transcript_text(sink,"\n\n");
        if(options->include_message_names) {
            transcript_text(sink,"Name:\n\n");transcript_literal(sink,message->name);
        }
        transcript_text(sink,"Text:\n\n");transcript_literal(sink,message->text);
    }
}

UmiStatus UmiAiConversationTranscriptMeasure(const UmiAiConversationRecord *record,
    const UmiAiConversationTranscriptOptions *options,size_t *out_bytes)
{
    if(out_bytes==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=transcript_validate(record,options);
    if(status!=UMI_STATUS_OK) return status;
    TranscriptSink sink={NULL,0U,UMI_STATUS_OK};
    transcript_emit(record,options,&sink);
    if(sink.status!=UMI_STATUS_OK) return sink.status;
    *out_bytes=sink.length;return UMI_STATUS_OK;
}

UmiStatus UmiAiConversationTranscriptMarkdown(const UmiAiConversationRecord *record,
    const UmiAiConversationTranscriptOptions *options,char *output,
    size_t capacity,size_t *out_bytes)
{
    if(output==NULL || out_bytes==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t bytes=0U;
    UmiStatus status=UmiAiConversationTranscriptMeasure(record,options,&bytes);
    if(status!=UMI_STATUS_OK) return status;
    if(capacity<=bytes) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Inputs remain immutable for this call. Admission and the complete count
     * pass finish before the first destination byte is changed. The identical
     * second pass has no allocations, callbacks or new fallible dependencies. */
    TranscriptSink sink={output,0U,UMI_STATUS_OK};
    transcript_emit(record,options,&sink);
    output[bytes]='\0';*out_bytes=bytes;
    return UMI_STATUS_OK;
}
