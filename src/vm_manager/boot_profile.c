/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vm_manager/boot_profile.c
 * PURPOSE: Persist boot configuration with optimistic revisions and atomic Data Server ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "boot_internal.h"
#include "umicom/vm_manager/boot_profile.h"
#include <stddef.h>
#include <limits.h>

#define BOOT_PROFILE_BYTES 32768U
static const char BootProfilePrefix[] = "vm.boot.profile.";
static const char BootProfileHeader[] = "UMICOM_VM_BOOT_PROFILE\t1\n";

typedef struct BootProfileText {
    const char *key;
    size_t offset, capacity;
} BootProfileText;

/* Store path bytes as hexadecimal so tabs, separators and Unicode never change
 * record boundaries. Runtime validation still rejects disallowed path bytes. */
static const BootProfileText ProfileText[] = {
    {"name", offsetof(UmiVmBootProfile, name), 128U},
    {"executable", offsetof(UmiVmBootProfile, request.executable), UMI_VM_PATH},
    {"directory", offsetof(UmiVmBootProfile, request.workingDirectory), UMI_VM_PATH},
    {"firmware", offsetof(UmiVmBootProfile, request.firmware), UMI_VM_PATH},
    {"kernel", offsetof(UmiVmBootProfile, request.kernel), UMI_VM_PATH},
    {"initrd", offsetof(UmiVmBootProfile, request.initrd), UMI_VM_PATH},
    {"disk", offsetof(UmiVmBootProfile, request.disk), UMI_VM_PATH},
    {"iso", offsetof(UmiVmBootProfile, request.iso), UMI_VM_PATH},
    {"append", offsetof(UmiVmBootProfile, request.commandLine), UMI_VM_BOOT_COMMAND_LINE}
};

static int ProfileValid(const UmiVmBootProfile *profile)
{
    return profile && memchr(profile->id, 0, sizeof profile->id) && VmId(profile->id) &&
        memchr(profile->name, 0, sizeof profile->name) && profile->name[0] &&
        VmUtf8(profile->name, sizeof profile->name) &&
        UmiVmBootRequestValidate(&profile->request) == UMI_STATUS_OK;
}

static void ProfileKey(const char *id, char key[96])
{
    (void)snprintf(key, 96U, "%s%s", BootProfilePrefix, id);
}

static void ProfileEncode(const UmiVmBootProfile *profile, uint64_t revision, VmText *text)
{
    static const char hex[] = "0123456789abcdef";
    VmTextPrint(text, "%sid\t%s\nrevision\t%" PRIu64 "\ntarget\t%u\nmemory\t%u\ncpus\t%u\n",
        BootProfileHeader, profile->id, revision, (unsigned)profile->request.target,
        profile->request.memoryMiB, profile->request.processors);
    for (size_t index = 0; index < sizeof ProfileText / sizeof ProfileText[0]; ++index) {
        const char *value = (const char *)profile + ProfileText[index].offset;
        VmTextPrint(text, "%s\t", ProfileText[index].key);
        if (!value[0]) VmTextPrint(text, "-");
        for (size_t byte = 0; value[byte]; ++byte) {
            unsigned number = (unsigned char)value[byte];
            VmTextPrint(text, "%c%c", hex[number >> 4U], hex[number & 15U]);
        }
        VmTextPrint(text, "\n");
    }
    if (text->used >= BOOT_PROFILE_BYTES) text->status = UMI_STATUS_CAPACITY_EXCEEDED;
}

/* The codec expects every field once in a fixed order. Unknown trailing data
 * and duplicate keys are rejected rather than silently changing launch intent. */
static char *ProfileLine(char **cursor, const char *key)
{
    char *line = *cursor, *end = strchr(line, '\n');
    if (!end) return NULL;
    *end = 0;
    *cursor = end + 1;
    size_t length = strlen(key);
    return strlen(line) > length && strncmp(line, key, length) == 0 &&
        line[length] == '\t' ? line + length + 1U : NULL;
}

static int ProfileHex(char byte)
{
    if (byte >= '0' && byte <= '9') return byte - '0';
    if (byte >= 'a' && byte <= 'f') return byte - 'a' + 10;
    return -1;
}

static int ProfileUnhex(const char *source, char *out, size_t capacity)
{
    if (strcmp(source, "-") == 0) { out[0] = 0; return 1; }
    size_t length = strlen(source);
    if (!length || length % 2U || length / 2U >= capacity) return 0;
    for (size_t index = 0; index < length; index += 2U) {
        int high = ProfileHex(source[index]), low = ProfileHex(source[index + 1U]);
        if (high < 0 || low < 0 || (high == 0 && low == 0)) return 0;
        out[index / 2U] = (char)((high << 4) | low);
    }
    out[length / 2U] = 0;
    return 1;
}

static UmiStatus ProfileDecode(char *text, UmiVmBootProfile *out)
{
    if (strncmp(text, BootProfileHeader, sizeof BootProfileHeader - 1U) != 0)
        return UMI_STATUS_PARSE_ERROR;
    char *cursor = text + sizeof BootProfileHeader - 1U;
    UmiVmBootProfile profile = {0};
    char *field = ProfileLine(&cursor, "id");
    if (!field || !VmId(field)) return UMI_STATUS_PARSE_ERROR;
    strcpy(profile.id, field);
    field = ProfileLine(&cursor, "revision");
    if (!field || !VmNumber(field, &profile.revision) || !profile.revision) return UMI_STATUS_PARSE_ERROR;
    uint64_t number;
    field = ProfileLine(&cursor, "target");
    if (!field || !VmNumber(field, &number) || number < UMI_VM_BOOT_UMICOM_KERNEL ||
        number > UMI_VM_BOOT_PC_DISK) return UMI_STATUS_PARSE_ERROR;
    profile.request.target = (UmiVmBootTarget)number;
    field = ProfileLine(&cursor, "memory");
    if (!field || !VmNumber(field, &number) || number > UINT_MAX) return UMI_STATUS_PARSE_ERROR;
    profile.request.memoryMiB = (unsigned)number;
    field = ProfileLine(&cursor, "cpus");
    if (!field || !VmNumber(field, &number) || number > UINT_MAX) return UMI_STATUS_PARSE_ERROR;
    profile.request.processors = (unsigned)number;
    for (size_t index = 0; index < sizeof ProfileText / sizeof ProfileText[0]; ++index) {
        field = ProfileLine(&cursor, ProfileText[index].key);
        if (!field || !ProfileUnhex(field, (char *)&profile + ProfileText[index].offset,
            ProfileText[index].capacity)) return UMI_STATUS_PARSE_ERROR;
    }
    if (*cursor || !ProfileValid(&profile)) return UMI_STATUS_PARSE_ERROR;
    *out = profile;
    return UMI_STATUS_OK;
}

UmiStatus UmiVmBootProfileLoad(const UmiDataServer *server, const char *id, UmiVmBootProfile *out)
{
    if (!server || !VmId(id) || !out) return UMI_STATUS_INVALID_ARGUMENT;
    char key[96];
    ProfileKey(id, key);
    char *text = malloc(BOOT_PROFILE_BYTES);
    if (!text) return UMI_STATUS_OUT_OF_MEMORY;
    UmiVmBootProfile profile;
    UmiStatus status = umi_data_server_get(server, key, text, BOOT_PROFILE_BYTES);
    if (status == UMI_STATUS_OK) status = ProfileDecode(text, &profile);
    if (status == UMI_STATUS_OK && strcmp(profile.id, id) != 0) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) *out = profile;
    free(text);
    return status;
}

typedef struct BootVisit {
    UmiVmBootProfileVisitor visitor;
    void *context;
    size_t count;
} BootVisit;

static UmiStatus ProfileVisit(const char *key, const char *value, void *context)
{
    BootVisit *visit = context;
    if (strncmp(key, BootProfilePrefix, sizeof BootProfilePrefix - 1U) != 0) return UMI_STATUS_OK;
    if (++visit->count > UMI_VM_MAX_PROFILES) return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t length = strlen(value);
    if (length >= BOOT_PROFILE_BYTES) return UMI_STATUS_PARSE_ERROR;
    char *copy = malloc(length + 1U);
    if (!copy) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(copy, value, length + 1U);
    UmiVmBootProfile profile;
    UmiStatus status = ProfileDecode(copy, &profile);
    free(copy);
    if (status == UMI_STATUS_OK && strcmp(key + sizeof BootProfilePrefix - 1U, profile.id) != 0)
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK && visit->visitor) status = visit->visitor(&profile, visit->context);
    return status;
}

UmiStatus UmiVmBootProfileVisit(const UmiDataServer *server, UmiVmBootProfileVisitor visitor, void *context)
{
    if (!server || !visitor) return UMI_STATUS_INVALID_ARGUMENT;
    BootVisit visit = {visitor, context, 0U};
    return umi_data_server_visit(server, ProfileVisit, &visit);
}

/* End only the transaction this service began. A rollback failure is reported
 * distinctly because the connection can no longer promise a clean boundary. */
static UmiStatus ProfileEnd(UmiDataServer *server, UmiStatus status)
{
    if (status == UMI_STATUS_OK) status = umi_data_server_commit(server);
    if (status != UMI_STATUS_OK && umi_data_server_rollback(server) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_STATE;
    return status;
}

UmiStatus UmiVmBootProfileSave(UmiDataServer *server, const UmiVmBootProfile *profile,
                             uint64_t expected, uint64_t *outRevision)
{
    if (!server || !outRevision || expected == UINT64_MAX || !ProfileValid(profile))
        return UMI_STATUS_INVALID_ARGUMENT;
    *outRevision = 0U;
    if (umi_data_server_in_transaction(server)) return UMI_STATUS_BUSY;
    UmiStatus status = umi_data_server_begin(server);
    if (status != UMI_STATUS_OK) return status;
    UmiVmBootProfile previous;
    status = UmiVmBootProfileLoad(server, profile->id, &previous);
    if (status == UMI_STATUS_NOT_FOUND) {
        status = expected ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK;
        if (status == UMI_STATUS_OK) {
            BootVisit visit = {NULL, NULL, 0U};
            status = umi_data_server_visit(server, ProfileVisit, &visit);
            if (status == UMI_STATUS_OK && visit.count == UMI_VM_MAX_PROFILES)
                status = UMI_STATUS_CAPACITY_EXCEEDED;
        }
    } else if (status == UMI_STATUS_OK && previous.revision != expected) {
        status = UMI_STATUS_INVALID_STATE;
    }
    VmText text;
    VmTextInit(&text);
    if (status == UMI_STATUS_OK) {
        ProfileEncode(profile, expected + 1U, &text);
        status = text.status;
        char key[96];
        ProfileKey(profile->id, key);
        if (status == UMI_STATUS_OK) status = umi_data_server_set(server, key, text.data);
    }
    VmTextFree(&text);
    status = ProfileEnd(server, status);
    if (status == UMI_STATUS_OK) *outRevision = expected + 1U;
    return status;
}

UmiStatus UmiVmBootProfileRemove(UmiDataServer *server, const char *id, uint64_t expected)
{
    if (!server || !VmId(id) || !expected) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_data_server_in_transaction(server)) return UMI_STATUS_BUSY;
    UmiStatus status = umi_data_server_begin(server);
    if (status != UMI_STATUS_OK) return status;
    UmiVmBootProfile profile;
    status = UmiVmBootProfileLoad(server, id, &profile);
    if (status == UMI_STATUS_OK && profile.revision != expected) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) {
        char key[96];
        ProfileKey(id, key);
        status = umi_data_server_delete(server, key);
    }
    return ProfileEnd(server, status);
}
