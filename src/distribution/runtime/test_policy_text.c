/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/test_policy_text.c
 * PURPOSE:
 *   Read and emit immutable, context-bound test policy documents.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: src/distribution/runtime/test_policy_text.c
 * Purpose: Read and emit immutable, context-bound test policy documents.
 *---------------------------------------------------------------------------*/
#include "test_policy_internal.h"
#include "inventory_codec.h"
#include <stdlib.h>
#include <string.h>

bool UmiReleaseTestPolicyNonblank(const char *text)
{
    for (const unsigned char *p = (const unsigned char *)text; *p != 0U; ++p)
        if (*p > 32U && *p != 127U) return true;
    return false;
}
static int Compare(const void *a, const void *b)
{ return strcmp(((const UmiReleaseTestRule *)a)->name, ((const UmiReleaseTestRule *)b)->name); }
void UmiReleaseTestPolicyDestroy(UmiReleaseTestPolicy *policy)
{ if (policy != NULL) { free(policy->rules); free(policy->storage); free(policy); } }
size_t UmiReleaseTestPolicyCount(const UmiReleaseTestPolicy *policy)
{ return policy != NULL ? policy->count : 0U; }
const UmiReleaseTestRule *UmiReleaseTestPolicyAt(const UmiReleaseTestPolicy *policy, size_t index)
{ return policy != NULL && index < policy->count ? &policy->rules[index] : NULL; }
const UmiReleaseTestRule *UmiReleaseTestPolicyFind(const UmiReleaseTestPolicy *policy, const char *name)
{
    size_t first = 0U, last = policy->count;
    while (first < last) {
        size_t middle = first + (last - first) / 2U;
        int order = strcmp(policy->rules[middle].name, name);
        if (order == 0) return &policy->rules[middle];
        if (order < 0) first = middle + 1U; else last = middle;
    }
    return NULL;
}

UmiStatus UmiReleaseTestPolicyParse(const char *text, size_t length, UmiReleaseTestPolicy **outPolicy)
{
    if (text == NULL || outPolicy == NULL || *outPolicy != NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_RELEASE_INVENTORY_TEXT_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length == 0U || memchr(text, '\0', length) != NULL) return UMI_STATUS_PARSE_ERROR;
    size_t lines = text[length - 1U] == '\n' ? 0U : 1U;
    for (size_t i = 0U; i < length; ++i) if (text[i] == '\n') ++lines;
    if (lines < 2U) return UMI_STATUS_PARSE_ERROR;
    if (lines - 2U > UMI_RELEASE_INVENTORY_RECORD_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiReleaseTestPolicy *policy = calloc(1U, sizeof(*policy));
    if (policy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    policy->storage = malloc(length + 1U);
    policy->rules = calloc(lines > 2U ? lines - 2U : 1U, sizeof(*policy->rules));
    if (policy->storage == NULL || policy->rules == NULL) {
        UmiReleaseTestPolicyDestroy(policy); return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(policy->storage, text, length); policy->storage[length] = '\0';
    UmiStatus status = UMI_STATUS_OK;
    char *cursor = policy->storage;
    for (size_t line = 0U; line < lines && status == UMI_STATUS_OK; ++line) {
        char *start = cursor, *end = strchr(start, '\n');
        if (end != NULL) { *end = '\0'; cursor = end + 1; }
        else cursor = start + strlen(start);
        size_t size = strlen(start);
        if (size != 0U && start[size - 1U] == '\r') start[size - 1U] = '\0';
        if (line == 0U) {
            if (strcmp(start, "UMICOM-RELEASE-TEST-POLICY\t1") != 0) status = UMI_STATUS_PARSE_ERROR;
            continue;
        }
        char *fields[6]; size_t count = UmiReleaseInventorySplitFields(start, fields, 6U);
        if (line == 1U) {
            if (count != 6U || strcmp(fields[0], "context") != 0 || strcmp(fields[1], "policy") != 0 ||
                !UmiReleaseInventoryGenerationValid(fields[2])) { status = UMI_STATUS_PARSE_ERROR; continue; }
            policy->generation = fields[2]; policy->sourceRoot = fields[3];
            policy->buildRoot = fields[4]; policy->configuration = fields[5];
            for (size_t i = 3U; i < 6U && status == UMI_STATUS_OK; ++i) {
                status = UmiReleaseInventoryDecodeField(fields[i]);
                if (status == UMI_STATUS_OK && fields[i][0] == '\0') status = UMI_STATUS_PARSE_ERROR;
            }
            continue;
        }
        if (count != 5U || strcmp(fields[0], "test") != 0) { status = UMI_STATUS_PARSE_ERROR; continue; }
        UmiReleaseTestRule *rule = &policy->rules[policy->count];
        rule->name = fields[1]; rule->owner = fields[2]; rule->reason = fields[4];
        if (strcmp(fields[3], "unassigned") == 0) rule->requirement = UMI_RELEASE_TEST_UNASSIGNED;
        else if (strcmp(fields[3], "required") == 0) rule->requirement = UMI_RELEASE_TEST_REQUIRED;
        else if (strcmp(fields[3], "optional") == 0) rule->requirement = UMI_RELEASE_TEST_OPTIONAL;
        else { status = UMI_STATUS_PARSE_ERROR; continue; }
        status = UmiReleaseInventoryDecodeField(fields[1]);
        if (status == UMI_STATUS_OK) status = UmiReleaseInventoryDecodeField(fields[2]);
        if (status == UMI_STATUS_OK) status = UmiReleaseInventoryDecodeField(fields[4]);
        if (status == UMI_STATUS_OK && (rule->name[0] == '\0' ||
            (rule->requirement != UMI_RELEASE_TEST_UNASSIGNED &&
             (!UmiReleaseTestPolicyNonblank(rule->owner) || !UmiReleaseTestPolicyNonblank(rule->reason)))))
            status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK) ++policy->count;
    }
    if (status == UMI_STATUS_OK) {
        qsort(policy->rules, policy->count, sizeof(*policy->rules), Compare);
        for (size_t i = 1U; i < policy->count; ++i)
            if (strcmp(policy->rules[i - 1U].name, policy->rules[i].name) == 0) { status = UMI_STATUS_ALREADY_EXISTS; break; }
    }
    if (status != UMI_STATUS_OK) { UmiReleaseTestPolicyDestroy(policy); return status; }
    *outPolicy = policy; return UMI_STATUS_OK;
}

/* Two passes compute the exact bounded allocation, then write it. The caller
 * sees no partial output if validation, capacity or allocation fails. */
typedef struct Writer { char *text; size_t used; UmiStatus status; } Writer;
static void Bytes(Writer *writer, const char *text, size_t size)
{
    if (writer->status != UMI_STATUS_OK) return;
    if (size > UMI_RELEASE_INVENTORY_TEXT_LIMIT - writer->used) { writer->status = UMI_STATUS_CAPACITY_EXCEEDED; return; }
    if (writer->text != NULL) memcpy(writer->text + writer->used, text, size);
    writer->used += size;
}
static void Literal(Writer *writer, const char *text) { Bytes(writer, text, strlen(text)); }
static void Encoded(Writer *writer, const char *text)
{
    static const char digits[] = "0123456789abcdef";
    size_t size = 0U;
    while (size <= UMI_RELEASE_INVENTORY_FIELD_LIMIT && text[size] != '\0') ++size;
    if (size > UMI_RELEASE_INVENTORY_FIELD_LIMIT) { writer->status = UMI_STATUS_CAPACITY_EXCEEDED; return; }
    if (size * 2U > UMI_RELEASE_INVENTORY_TEXT_LIMIT - writer->used) { writer->status = UMI_STATUS_CAPACITY_EXCEEDED; return; }
    if (writer->status != UMI_STATUS_OK) return;
    if (writer->text != NULL) {
        for (size_t i = 0U; i < size; ++i) {
            unsigned char c = (unsigned char)text[i];
            writer->text[writer->used + 2U * i] = digits[c >> 4U];
            writer->text[writer->used + 2U * i + 1U] = digits[c & 15U];
        }
    }
    writer->used += size * 2U;
}
static void Context(Writer *writer, const char *generation, const char *source, const char *build, const char *config)
{
    Literal(writer, "UMICOM-RELEASE-TEST-POLICY\t1\ncontext\tpolicy\t"); Literal(writer, generation);
    Literal(writer, "\t"); Encoded(writer, source); Literal(writer, "\t"); Encoded(writer, build);
    Literal(writer, "\t"); Encoded(writer, config); Literal(writer, "\n");
}
static void Rule(Writer *writer, const UmiReleaseTestRule *rule)
{
    Literal(writer, "test\t"); Encoded(writer, rule->name); Literal(writer, "\t"); Encoded(writer, rule->owner);
    Literal(writer, rule->requirement == UMI_RELEASE_TEST_REQUIRED ? "\trequired\t" :
        rule->requirement == UMI_RELEASE_TEST_OPTIONAL ? "\toptional\t" : "\tunassigned\t");
    Encoded(writer, rule->reason); Literal(writer, "\n");
}
static UmiStatus Allocate(Writer *writer)
{
    if (writer->status != UMI_STATUS_OK) return writer->status;
    writer->text = malloc(writer->used + 1U);
    if (writer->text == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    writer->used = 0U; return UMI_STATUS_OK;
}
void UmiReleaseTestPolicyTextDestroy(char *text) { free(text); }
UmiStatus UmiReleaseTestPolicyDraft(const UmiReleaseInventory *configured, char **outText, size_t *outLength)
{
    if (configured == NULL || outText == NULL || *outText != NULL || outLength == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(UmiReleaseInventoryProducer(configured), "cmake") != 0) return UMI_STATUS_INVALID_STATE;
    UmiReleaseInventorySummary summary = {0}; (void)UmiReleaseInventorySummarise(configured, &summary);
    if (summary.tests == 0U) return UMI_STATUS_UNAVAILABLE;
    Writer writer = {0};
    for (unsigned pass = 0U; pass < 2U; ++pass) {
        Context(&writer, UmiReleaseInventoryGeneration(configured), UmiReleaseInventorySourceRoot(configured),
            UmiReleaseInventoryBuildRoot(configured), UmiReleaseInventoryConfiguration(configured));
        for (size_t i = 0U; i < UmiReleaseInventoryCount(configured); ++i) {
            const UmiReleaseInventoryRecord *row = UmiReleaseInventoryAt(configured, i);
            if (row->kind == UMI_RELEASE_INVENTORY_TEST) {
                UmiReleaseTestRule rule = { row->identity, "", "", UMI_RELEASE_TEST_UNASSIGNED }; Rule(&writer, &rule);
            }
        }
        if (pass == 0U) { UmiStatus status = Allocate(&writer); if (status != UMI_STATUS_OK) return status; }
    }
    if (writer.status != UMI_STATUS_OK) { free(writer.text); return writer.status; }
    writer.text[writer.used] = '\0'; *outText = writer.text; *outLength = writer.used; return UMI_STATUS_OK;
}
UmiStatus UmiReleaseTestPolicyEdit(const UmiReleaseTestPolicy *policy, const char *name,
    UmiReleaseTestRequirement requirement, const char *owner, const char *reason, char **outText, size_t *outLength)
{
    if (policy == NULL || name == NULL || owner == NULL || reason == NULL || outText == NULL ||
        *outText != NULL || outLength == NULL || (requirement != UMI_RELEASE_TEST_UNASSIGNED &&
        requirement != UMI_RELEASE_TEST_REQUIRED && requirement != UMI_RELEASE_TEST_OPTIONAL))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (requirement != UMI_RELEASE_TEST_UNASSIGNED &&
        (!UmiReleaseTestPolicyNonblank(owner) || !UmiReleaseTestPolicyNonblank(reason))) return UMI_STATUS_INVALID_ARGUMENT;
    const UmiReleaseTestRule *selected = UmiReleaseTestPolicyFind(policy, name);
    if (selected == NULL) return UMI_STATUS_NOT_FOUND;
    UmiReleaseTestRule replacement = { selected->name, owner, reason, requirement };
    Writer writer = {0};
    for (unsigned pass = 0U; pass < 2U; ++pass) {
        Context(&writer, policy->generation, policy->sourceRoot, policy->buildRoot, policy->configuration);
        for (size_t i = 0U; i < policy->count; ++i) Rule(&writer, &policy->rules[i] == selected ? &replacement : &policy->rules[i]);
        if (pass == 0U) { UmiStatus status = Allocate(&writer); if (status != UMI_STATUS_OK) return status; }
    }
    if (writer.status != UMI_STATUS_OK) { free(writer.text); return writer.status; }
    writer.text[writer.used] = '\0'; *outText = writer.text; *outLength = writer.used; return UMI_STATUS_OK;
}
