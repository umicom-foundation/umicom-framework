/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/internal.h
 * PURPOSE: Keep editing, wire encoding and transaction ownership private to Framework.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CREATIVE_INTERNAL_H
#define UMICOM_CREATIVE_INTERNAL_H
#include "umicom/creative_workspace/workspace.h"
#include "umicom/creative_workspace/export.h"
#define UMI_CREATIVE_WIRE_CAPACITY 4096U
struct UmiCreativeWorkspace {
    UmiDataServer * server;
    UmiCreativeProject current;
    UmiCreativeProject * history;
    size_t historyCount, historyCursor;
    bool recoveryRequired, busy;
};

bool UmiCreativeTextValid(const char * text, size_t capacity, bool allowEmpty);
bool UmiCreativeIdValid(const char * id);
UmiStatus UmiCreativeCopyText(char * out, size_t capacity, const char * text);
size_t UmiCreativeFindElementIndex(const UmiCreativeProject * project, const char * id);
size_t UmiCreativeFindNoteIndex(const UmiCreativeProject * project, const char * id);
bool UmiCreativeProjectEqual(const UmiCreativeProject * a, const UmiCreativeProject * b);
UmiStatus UmiCreativeStoreLoad(UmiDataServer * server, const char * id, UmiCreativeProject * out, bool * poisoned);
UmiStatus UmiCreativeStoreSave(UmiDataServer * server, const UmiCreativeProject * before,
const UmiCreativeProject * after, bool * poisoned);
UmiStatus UmiCreativeWireProjectWrite(const UmiCreativeProject * project, char * out, size_t capacity);
UmiStatus UmiCreativeWireProjectRead(const char * wire, UmiCreativeProject * out);
UmiStatus UmiCreativeWireElementWrite(const UmiCreativeElement * element, char * out, size_t capacity);
UmiStatus UmiCreativeWireElementRead(const char * wire, UmiCreativeElement * out);
UmiStatus UmiCreativeWireNoteWrite(const UmiCreativeNote * note, char * out, size_t capacity);
UmiStatus UmiCreativeWireNoteRead(const char * wire, UmiCreativeNote * out);
/* A bounded output writer centralises arithmetic and escapes all user text.
 * No exporter interprets a label as HTML, XML, a URL or executable content. */
typedef struct UmiCreativeWriter { unsigned char * bytes;
    size_t size, capacity;
    UmiStatus status;
} UmiCreativeWriter;
void UmiCreativeWriterInit(UmiCreativeWriter * writer, size_t capacity);
void UmiCreativeWriterAppend(UmiCreativeWriter * writer, const void * bytes, size_t size);
void UmiCreativeWriterText(UmiCreativeWriter * writer, const char * text);
void UmiCreativeWriterNumber(UmiCreativeWriter * writer, uint64_t value);
void UmiCreativeWriterXml(UmiCreativeWriter * writer, const char * text);
UmiStatus UmiCreativeWriterFinish(UmiCreativeWriter * writer, UmiCreativeExport * out);
UmiStatus UmiCreativeExportSvg(const UmiCreativeProject * project, uint32_t timeMs, UmiCreativeExport * out);
UmiStatus UmiCreativeExportHtml(const UmiCreativeProject * project, uint32_t timeMs, bool storyboard, UmiCreativeExport * out);
UmiStatus UmiCreativeExportWave(const UmiCreativeProject * project, UmiCreativeExport * out);
void UmiCreativeWriterBrandLogo(UmiCreativeWriter * writer);
void UmiCreativeWriterBrandIcon(UmiCreativeWriter * writer);
#endif
