/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/storage.c
 * PURPOSE: Persist whole creative edits atomically through Data Server transactions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Project IDs are validated before they become key components. No user text
 * reaches SQL and no application bypasses this repository to edit records. */
static UmiStatus Key(char * out, size_t capacity, const char * id, const char * family, size_t index)
{
    int count = snprintf(out, capacity, "creative.project/%s/%s/%zu", id, family, index);
    return count < 0 || (size_t)count >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}

static UmiStatus ReadRecord(UmiDataServer * server, const char * id, const char * family, size_t index, char * out)
{
    char key[192];
    UmiStatus status = Key(key, sizeof(key), id, family, index);
    return status == UMI_STATUS_OK ? umi_data_server_get(server, key, out, UMI_CREATIVE_WIRE_CAPACITY) : status;
}

static UmiStatus WriteRecord(UmiDataServer * server, const char * id, const char * family, size_t index, const char * wire)
{
    char key[192];
    UmiStatus status = Key(key, sizeof(key), id, family, index);
    return status == UMI_STATUS_OK ? umi_data_server_set(server, key, wire) : status;
}

static UmiStatus RemoveRecord(UmiDataServer * server, const char * id, const char * family, size_t index)
{
    char key[192];
    UmiStatus status = Key(key, sizeof(key), id, family, index);
    if (status == UMI_STATUS_OK)status = umi_data_server_delete(server, key);
    return status == UMI_STATUS_NOT_FOUND ? UMI_STATUS_PARSE_ERROR : status;
}

static UmiStatus ReadProject(UmiDataServer * server, const char * id, UmiCreativeProject * out)
{
    char wire[UMI_CREATIVE_WIRE_CAPACITY];
    UmiStatus status = ReadRecord(server, id, "meta", 0U, wire);
    if (status!= UMI_STATUS_OK)return status;
    status = UmiCreativeWireProjectRead(wire, out);
    if (status == UMI_STATUS_OK && strcmp(out -> id, id)!= 0)status = UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0U;status == UMI_STATUS_OK && i < out -> elementCount; ++ i){
        status = ReadRecord(server, id, "element", i, wire);
        if (status == UMI_STATUS_OK)status = UmiCreativeWireElementRead(wire, & out -> elements[i]);
        if (status == UMI_STATUS_NOT_FOUND)status = UMI_STATUS_PARSE_ERROR;
    }
    for (size_t i = 0U;status == UMI_STATUS_OK && i < out -> noteCount; ++ i){
        status = ReadRecord(server, id, "note", i, wire);
        if (status == UMI_STATUS_OK)status = UmiCreativeWireNoteRead(wire, & out -> notes[i]);
        if (status == UMI_STATUS_NOT_FOUND)status = UMI_STATUS_PARSE_ERROR;
    }
    if (status == UMI_STATUS_OK && UmiCreativeProjectValidate(out)!= UMI_STATUS_OK)status = UMI_STATUS_PARSE_ERROR;
    return status;
}

/* Only a transaction successfully begun by this operation reaches Finish.
 * A failed rollback poisons the owner instead of presenting cached data as saved. */
static UmiStatus Finish(UmiDataServer * server, UmiStatus status, bool * poisoned)
{
    if (status == UMI_STATUS_OK)status = umi_data_server_commit(server);
    if (status!= UMI_STATUS_OK && umi_data_server_rollback(server)!= UMI_STATUS_OK){
        * poisoned = true;
        return UMI_STATUS_INVALID_STATE;
    }
    return status;
}

UmiStatus UmiCreativeStoreLoad(UmiDataServer * server, const char * id, UmiCreativeProject * out, bool * poisoned)
{
    if (server == NULL || out == NULL || poisoned == NULL || !UmiCreativeIdValid(id))return UMI_STATUS_INVALID_ARGUMENT;
    if ( * poisoned)return UMI_STATUS_INVALID_STATE;
    UmiCreativeProject * candidate = calloc(1U, sizeof( * candidate));
    if (candidate == NULL)return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = umi_data_server_begin(server);
    if (status == UMI_STATUS_OK){
        status = ReadProject(server, id, candidate);
        status = Finish(server, status, poisoned);
    }
    if (status == UMI_STATUS_OK) * out = * candidate;
    free(candidate);
    return status;
}

UmiStatus UmiCreativeStoreSave(UmiDataServer * server, const UmiCreativeProject * before,
const UmiCreativeProject * after, bool * poisoned)
{
    if (server == NULL || poisoned == NULL || UmiCreativeProjectValidate(after)!= UMI_STATUS_OK || after -> revision == 0U)return UMI_STATUS_INVALID_ARGUMENT;
    if ( * poisoned)return UMI_STATUS_INVALID_STATE;
    UmiCreativeProject * stored = calloc(1U, sizeof( * stored));
    if (stored == NULL)return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = umi_data_server_begin(server);
    if (status!= UMI_STATUS_OK){
        free(stored);
        return status;
    }
    status = ReadProject(server, after -> id, stored);
    if (before == NULL){
        if (status == UMI_STATUS_OK)status = UMI_STATUS_ALREADY_EXISTS;
        else if (status == UMI_STATUS_NOT_FOUND)status = UMI_STATUS_OK;
    }
    else{
        if (status == UMI_STATUS_NOT_FOUND)status = UMI_STATUS_BUSY;
        if (status == UMI_STATUS_OK && (stored -> revision!= before -> revision || !UmiCreativeProjectEqual(stored, before)))status = UMI_STATUS_BUSY;
    }
    bool changing = before == NULL || !UmiCreativeProjectEqual(before, after);
    char wire[UMI_CREATIVE_WIRE_CAPACITY];
    for (size_t i = 0U;status == UMI_STATUS_OK && changing && i < after -> elementCount; ++ i){
        status = UmiCreativeWireElementWrite( & after -> elements[i], wire, sizeof(wire));
        if (status == UMI_STATUS_OK)status = WriteRecord(server, after -> id, "element", i, wire);
    }
    for (size_t i = 0U;status == UMI_STATUS_OK && changing && i < after -> noteCount; ++ i){
        status = UmiCreativeWireNoteWrite( & after -> notes[i], wire, sizeof(wire));
        if (status == UMI_STATUS_OK)status = WriteRecord(server, after -> id, "note", i, wire);
    }
    if (before!= NULL && changing){
        for (size_t i = after -> elementCount;status == UMI_STATUS_OK && i < before -> elementCount; ++ i)status = RemoveRecord(server, after -> id, "element", i);
        for (size_t i = after -> noteCount;status == UMI_STATUS_OK && i < before -> noteCount; ++ i)status = RemoveRecord(server, after -> id, "note", i);
    }
    if (status == UMI_STATUS_OK && changing){
        status = UmiCreativeWireProjectWrite(after, wire, sizeof(wire));
        if (status == UMI_STATUS_OK)status = WriteRecord(server, after -> id, "meta", 0U, wire);
    }
    status = Finish(server, status, poisoned);
    free(stored);
    return status;
}

