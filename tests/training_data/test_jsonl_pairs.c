/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_data/test_jsonl_pairs.c
 *
 * PURPOSE:
 *   Verify strict pair schemas, ownership, rejection atomicity and decoded text limits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/jsonl_pairs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)

static const char valid[] = "{\"source_id\":\"guide-1\",\"prompt\":\"Why?\",\"completion\":\"Because.\"}";
static int Refuse(const char *input, size_t length, UmiStatus expected)
{
    UmiAiTrainingPair value, before;
    memset(&value, 0x5a, sizeof(value)); before = value;
    CHECK(UmiAiTrainingPairRead(input, length, NULL, &value) == expected);
    CHECK(memcmp(&value, &before, sizeof(value)) == 0);
    return 0;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    struct { const char *name, *input; UmiStatus expected; } cases[] = {
        {"array", "[]", UMI_STATUS_PARSE_ERROR},
        {"null", "null", UMI_STATUS_PARSE_ERROR},
        {"missing", "{\"source_id\":\"a\",\"completion\":\"b\"}", UMI_STATUS_PARSE_ERROR},
        {"extra", "{\"source_id\":\"a\",\"prompt\":\"b\",\"completion\":\"c\",\"unknown\":1}", UMI_STATUS_PARSE_ERROR},
        {"unknown", "{\"source_id\":\"a\",\"prompt\":\"b\",\"answer\":\"c\"}", UMI_STATUS_PARSE_ERROR},
        {"wrong-type", "{\"source_id\":\"a\",\"prompt\":42,\"completion\":\"c\"}", UMI_STATUS_PARSE_ERROR},
        {"nested", "{\"source_id\":\"a\",\"prompt\":{},\"completion\":\"c\"}", UMI_STATUS_PARSE_ERROR},
        {"empty-source", "{\"source_id\":\"\",\"prompt\":\"b\",\"completion\":\"c\"}", UMI_STATUS_PARSE_ERROR},
        {"empty-completion", "{\"source_id\":\"a\",\"prompt\":\"b\",\"completion\":\"\"}", UMI_STATUS_PARSE_ERROR},
        {"duplicate", "{\"source_id\":\"a\",\"prompt\":\"b\",\"prompt\":\"c\"}", UMI_STATUS_ALREADY_EXISTS},
        {"escaped-duplicate", "{\"source_id\":\"a\",\"prompt\":\"b\",\"pr\\u006fmpt\":\"c\"}", UMI_STATUS_ALREADY_EXISTS},
        {"decoded-nul", "{\"source_id\":\"a\",\"prompt\":\"b\",\"completion\":\"\\u0000\"}", UMI_STATUS_PARSE_ERROR},
        {"surrogate", "{\"source_id\":\"a\",\"prompt\":\"b\",\"completion\":\"\\ud800\"}", UMI_STATUS_PARSE_ERROR},
        {"invalid-utf8", "{\"source_id\":\"a\",\"prompt\":\"b\",\"completion\":\"\xc0\xaf\"}", UMI_STATUS_PARSE_ERROR},
        {"trailing", "{\"source_id\":\"a\",\"prompt\":\"b\",\"completion\":\"c\"}[]", UMI_STATUS_PARSE_ERROR},
        {"bom", "\xef\xbb\xbf{\"source_id\":\"a\",\"prompt\":\"b\",\"completion\":\"c\"}", UMI_STATUS_PARSE_ERROR}
    };
    for (size_t i = 0; i < sizeof(cases)/sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i].name) == 0)
            return Refuse(cases[i].input, strlen(cases[i].input), cases[i].expected);
    UmiAiTrainingPair value;
    if (strcmp(mode, "valid") == 0) {
        CHECK(UmiAiTrainingPairRead(valid, sizeof(valid)-1U, NULL, &value) == UMI_STATUS_OK);
        CHECK(strcmp(value.sourceId, "guide-1") == 0);
        CHECK(strcmp(value.prompt, "Why?") == 0 && strcmp(value.completion, "Because.") == 0);
    } else if (strcmp(mode, "owned-copy") == 0) {
        char input[sizeof(valid)]; memcpy(input, valid, sizeof(valid));
        CHECK(UmiAiTrainingPairRead(input, sizeof(input)-1U, NULL, &value) == UMI_STATUS_OK);
        memset(input, 0, sizeof(input));
        CHECK(strcmp(value.completion, "Because.") == 0);
    } else if (strcmp(mode, "unicode-escapes") == 0) {
        const char text[] = "{\"completion\":\"caf\\u00e9\\n\\ud83d\\ude42\",\"prompt\":\"\",\"source_id\":\"z\"}";
        CHECK(UmiAiTrainingPairRead(text, sizeof(text)-1U, NULL, &value) == UMI_STATUS_OK);
        CHECK(strcmp(value.completion, "caf\xc3\xa9\n\xf0\x9f\x99\x82") == 0 && value.prompt[0] == 0);
    } else if (strcmp(mode, "embedded-nul") == 0) {
        char input[sizeof(valid)]; memcpy(input, valid, sizeof(valid)); input[9] = 0;
        return Refuse(input, sizeof(input)-1U, UMI_STATUS_PARSE_ERROR);
    } else if (strcmp(mode, "limits") == 0) {
        char *input = malloc(UMI_AI_TRAINING_PAIR_TEXT_BYTES + 200U); CHECK(input != NULL);
        const char *prefix = "{\"source_id\":\"a\",\"prompt\":\"\",\"completion\":\"";
        size_t used = strlen(prefix); memcpy(input, prefix, used);
        memset(input+used, 'x', UMI_AI_TRAINING_PAIR_TEXT_BYTES);
        memcpy(input+used+UMI_AI_TRAINING_PAIR_TEXT_BYTES, "\"}", 3U);
        CHECK(UmiAiTrainingPairRead(input, strlen(input), NULL, &value) == UMI_STATUS_OK);
        CHECK(strlen(value.completion) == UMI_AI_TRAINING_PAIR_TEXT_BYTES);
        input[used+UMI_AI_TRAINING_PAIR_TEXT_BYTES] = 'x';
        memcpy(input+used+UMI_AI_TRAINING_PAIR_TEXT_BYTES+1U, "\"}", 3U);
        CHECK(Refuse(input, strlen(input), UMI_STATUS_CAPACITY_EXCEEDED) == 0);
        free(input);
    } else if (strcmp(mode, "source-limit") == 0) {
        char input[256]; const char *prefix = "{\"source_id\":\"";
        size_t used = strlen(prefix); memcpy(input,prefix,used);
        memset(input+used,'a',UMI_AI_TRAINING_PAIR_SOURCE_BYTES);
        strcpy(input+used+UMI_AI_TRAINING_PAIR_SOURCE_BYTES,"\",\"prompt\":\"p\",\"completion\":\"c\"}");
        CHECK(UmiAiTrainingPairRead(input,strlen(input),NULL,&value)==UMI_STATUS_OK);
        input[used+UMI_AI_TRAINING_PAIR_SOURCE_BYTES]='b';
        strcpy(input+used+UMI_AI_TRAINING_PAIR_SOURCE_BYTES+1U,"\",\"prompt\":\"p\",\"completion\":\"c\"}");
        CHECK(Refuse(input,strlen(input),UMI_STATUS_CAPACITY_EXCEEDED)==0);
    } else if (strcmp(mode, "cancel") == 0) {
        UmiCancellationToken *token = NULL; CHECK(umi_cancellation_token_create(&token) == UMI_STATUS_OK);
        umi_cancellation_token_request(token);
        memset(&value,0x5a,sizeof(value)); UmiAiTrainingPair before=value;
        CHECK(UmiAiTrainingPairRead(valid,sizeof(valid)-1U,token,&value)==UMI_STATUS_CANCELLED);
        CHECK(memcmp(&before,&value,sizeof(value))==0); umi_cancellation_token_destroy(token);
    } else if (strcmp(mode, "arguments") == 0) {
        CHECK(Refuse(NULL, 1U, UMI_STATUS_INVALID_ARGUMENT)==0);
        CHECK(Refuse(valid, 0U, UMI_STATUS_INVALID_ARGUMENT)==0);
        CHECK(Refuse(valid, UMI_AI_TRAINING_PAIR_LINE_BYTES+1U, UMI_STATUS_CAPACITY_EXCEEDED)==0);
        CHECK(UmiAiTrainingPairRead(valid,sizeof(valid)-1U,NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    } else return 2;
    return 0;
}
