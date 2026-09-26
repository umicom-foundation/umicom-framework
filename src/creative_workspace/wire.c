/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/wire.c
 * PURPOSE: Encode records as bounded, explicit fields rather than compiler-dependent structs.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
typedef struct WireWriter {
    char * text;
    size_t capacity, size;
    UmiStatus status;
}

WireWriter;
typedef struct WireReader {
    const char * text;
    size_t size, at;
    UmiStatus status;
}

WireReader;
static WireWriter Writer(char * out, size_t capacity)
{
    WireWriter w = {out, capacity, 0U, UMI_STATUS_OK};
    if (out == NULL || capacity == 0U)w.status = UMI_STATUS_INVALID_ARGUMENT;
    else out[0] = '\0';
    return w;
}

static void Add(WireWriter * w, const char * text)
{
    if (w -> status!= UMI_STATUS_OK)return;
    size_t n = strlen(text);
    if (n >= w -> capacity - w -> size){
        w -> status = UMI_STATUS_CAPACITY_EXCEEDED;
        return;
    }
    memcpy(w -> text + w -> size, text, n + 1U);
    w -> size += n;
}

static void Unsigned(WireWriter * w, uint64_t value)
{
    char text[32];
    (void)snprintf(text, sizeof(text), "%" PRIu64 "\n", value);
    Add(w, text);
}

static void Signed(WireWriter * w, int32_t value)
{
    char text[32];
    (void)snprintf(text, sizeof(text), "%" PRId32 "\n", value);
    Add(w, text);
}

static void Hex(WireWriter * w, const char * text, size_t capacity)
{
    static const char digits[] = "0123456789abcdef";
    if (!UmiCreativeTextValid(text, capacity, false)){
        w -> status = UMI_STATUS_INVALID_ARGUMENT;
        return;
    }
    for (size_t i = 0U;text[i]!= '\0'; ++ i){
        unsigned char c = (unsigned char)text[i];
        char pair[3] = {digits[c >> 4U], digits[c & 15U], 0};
        Add(w, pair);
    }
    Add(w, "\n");
}

static UmiStatus EndWrite(WireWriter * w)
{
    if (w -> status!= UMI_STATUS_OK && w -> text!= NULL && w -> capacity > 0U)w -> text[0] = '\0';
    return w -> status;
}

static WireReader Reader(const char * text)
{
    WireReader r = {text, 0U, 0U, UMI_STATUS_OK};
    if (text == NULL){
        r.status = UMI_STATUS_INVALID_ARGUMENT;
        return r;
    }
    while (r.size < UMI_CREATIVE_WIRE_CAPACITY && text[r.size]!= '\0') ++ r.size;
    if (r.size == UMI_CREATIVE_WIRE_CAPACITY)r.status = UMI_STATUS_PARSE_ERROR;
    return r;
}

static const char * Line(WireReader * r, size_t * size)
{
    * size = 0U;
    if (r -> status!= UMI_STATUS_OK)return "";
    size_t start = r -> at;
    while (r -> at < r -> size && r -> text[r -> at]!= '\n') ++ r -> at;
    if (r -> at == r -> size){
        r -> status = UMI_STATUS_PARSE_ERROR;
        return "";
    }
    * size = r -> at - start;
    ++ r -> at;
    return r -> text + start;
}

static void Header(WireReader * r, const char * expected)
{
    size_t n;
    const char * line = Line(r, & n);
    if (n!= strlen(expected) || memcmp(line, expected, n)!= 0)r -> status = UMI_STATUS_PARSE_ERROR;
}

static uint64_t Decimal(WireReader * r, const char * p, size_t n, uint64_t maximum)
{
    uint64_t value = 0U;
    if (n == 0U || n > 20U || (n > 1U && p[0] == '0')){
        r -> status = UMI_STATUS_PARSE_ERROR;
        return 0U;
    }
    for (size_t i = 0U;i < n; ++ i){
        if (p[i] < '0' || p[i] > '9'){
            r -> status = UMI_STATUS_PARSE_ERROR;
            return 0U;
        }
        uint64_t digit = (uint64_t)(p[i] - '0');
        if (digit > maximum || value > (maximum - digit) / 10U){
            r -> status = UMI_STATUS_PARSE_ERROR;
            return 0U;
        }
        value = value * 10U + digit;
    }
    return value;
}

static uint64_t Number(WireReader * r, uint64_t maximum)
{
    size_t n;
    const char * p = Line(r, & n);
    return Decimal(r, p, n, maximum);
}

static int32_t Integer(WireReader * r)
{
    size_t n;
    const char * p = Line(r, & n);
    bool negative = n > 0U && p[0] == '-';
    if (negative){
        ++ p;
        -- n;
    }
    uint64_t magnitude = Decimal(r, p, n, negative ? (uint64_t)INT32_MAX + 1U : (uint64_t)INT32_MAX);
    if (negative && magnitude == 0U)r -> status = UMI_STATUS_PARSE_ERROR;
    int64_t value = negative ? - (int64_t)magnitude : (int64_t)magnitude;
    return (int32_t)value;
}

static int Digit(char c)
{
    if (c >= '0' && c <= '9')return c - '0';
    if (c >= 'a' && c <= 'f')return c - 'a' + 10;
    return -1;
}

static void Text(WireReader * r, char * out, size_t capacity)
{
    size_t n;
    const char * p = Line(r, & n);
    if (n == 0U || n % 2U!= 0U || n / 2U >= capacity){
        r -> status = UMI_STATUS_PARSE_ERROR;
        return;
    }
    for (size_t i = 0U;i < n / 2U; ++ i){
        int a = Digit(p[i * 2U]), b = Digit(p[i * 2U + 1U]);
        if (a < 0 || b < 0 || (a == 0 && b == 0)){
            r -> status = UMI_STATUS_PARSE_ERROR;
            return;
        }
        out[i] = (char)(unsigned char)(a * 16 + b);
    }
    out[n / 2U] = '\0';
    if (!UmiCreativeTextValid(out, capacity, false))r -> status = UMI_STATUS_PARSE_ERROR;
}

static UmiStatus EndRead(WireReader * r)
{
    return r -> status == UMI_STATUS_OK && r -> at == r -> size ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}

UmiStatus UmiCreativeWireProjectWrite(const UmiCreativeProject * p, char * out, size_t capacity)
{
    WireWriter w = Writer(out, capacity);
    if (UmiCreativeProjectValidate(p)!= UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    Add( & w, "umicom.creative.project/1\n");
    Unsigned( & w, p -> revision);
    Hex( & w, p -> id, sizeof(p -> id));
    Hex( & w, p -> settings.title, sizeof(p -> settings.title));
    Unsigned( & w, (uint64_t)p -> settings.unit);
    Unsigned( & w, (uint64_t)p -> settings.width);
    Unsigned( & w, (uint64_t)p -> settings.height);
    Unsigned( & w, p -> settings.durationMs);
    Unsigned( & w, p -> settings.tempoBpm);
    Unsigned( & w, p -> elementCount);
    Unsigned( & w, p -> noteCount);
    return EndWrite( & w);
}

UmiStatus UmiCreativeWireProjectRead(const char * wire, UmiCreativeProject * out)
{
    if (out == NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof( * out));
    WireReader r = Reader(wire);
    Header( & r, "umicom.creative.project/1");
    out -> revision = Number( & r, UINT64_MAX);
    Text( & r, out -> id, sizeof(out -> id));
    Text( & r, out -> settings.title, sizeof(out -> settings.title));
    out -> settings.unit = (UmiCreativeUnit)Number( & r, 1U);
    out -> settings.width = (int32_t)Number( & r, UMI_CREATIVE_MAX_EXTENT);
    out -> settings.height = (int32_t)Number( & r, UMI_CREATIVE_MAX_EXTENT);
    out -> settings.durationMs = (uint32_t)Number( & r, UMI_CREATIVE_MAX_DURATION_MS);
    out -> settings.tempoBpm = (uint32_t)Number( & r, 300U);
    size_t elements = (size_t)Number( & r, UMI_CREATIVE_MAX_ELEMENTS), notes = (size_t)Number( & r, UMI_CREATIVE_MAX_NOTES);
    UmiStatus status = EndRead( & r);
    if (status == UMI_STATUS_OK && (out -> revision == 0U || UmiCreativeProjectValidate(out)!= UMI_STATUS_OK))status = UMI_STATUS_PARSE_ERROR;
    out -> elementCount = elements;
    out -> noteCount = notes;
    if (status!= UMI_STATUS_OK)memset(out, 0, sizeof( * out));
    return status;
}

UmiStatus UmiCreativeWireElementWrite(const UmiCreativeElement * e, char * out, size_t capacity)
{
    WireWriter w = Writer(out, capacity);
    if (e == NULL || e -> keyCount > UMI_CREATIVE_MAX_KEYS)return UMI_STATUS_INVALID_ARGUMENT;
    Add( & w, "umicom.creative.element/1\n");
    Hex( & w, e -> id, sizeof(e -> id));
    Hex( & w, e -> label, sizeof(e -> label));
    Unsigned( & w, (uint64_t)e -> kind);
    Signed( & w, e -> x);
    Signed( & w, e -> y);
    Signed( & w, e -> width);
    Signed( & w, e -> height);
    Unsigned( & w, e -> colourRgb);
    Unsigned( & w, e -> visible ? 1U : 0U);
    Unsigned( & w, e -> keyCount);
    for (size_t i = 0U;i < e -> keyCount; ++ i){
        Unsigned( & w, e -> keys[i].timeMs);
        Signed( & w, e -> keys[i].offsetX);
        Signed( & w, e -> keys[i].offsetY);
        Unsigned( & w, (uint64_t)e -> keys[i].interpolation);
    }
    return EndWrite( & w);
}

UmiStatus UmiCreativeWireElementRead(const char * wire, UmiCreativeElement * out)
{
    if (out == NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof( * out));
    WireReader r = Reader(wire);
    Header( & r, "umicom.creative.element/1");
    Text( & r, out -> id, sizeof(out -> id));
    Text( & r, out -> label, sizeof(out -> label));
    out -> kind = (UmiCreativeElementKind)Number( & r, 6U);
    out -> x = Integer( & r);
    out -> y = Integer( & r);
    out -> width = Integer( & r);
    out -> height = Integer( & r);
    out -> colourRgb = (uint32_t)Number( & r, 0xFFFFFFU);
    out -> visible = Number( & r, 1U)!= 0U;
    out -> keyCount = (size_t)Number( & r, UMI_CREATIVE_MAX_KEYS);
    for (size_t i = 0U;i < out -> keyCount; ++ i){
        out -> keys[i].timeMs = (uint32_t)Number( & r, UMI_CREATIVE_MAX_DURATION_MS);
        out -> keys[i].offsetX = Integer( & r);
        out -> keys[i].offsetY = Integer( & r);
        out -> keys[i].interpolation = (UmiMediaAnimationInterpolation)Number( & r, 1U);
    }
    UmiStatus status = EndRead( & r);
    if (status!= UMI_STATUS_OK)memset(out, 0, sizeof( * out));
    return status;
}

UmiStatus UmiCreativeWireNoteWrite(const UmiCreativeNote * note, char * out, size_t capacity)
{
    WireWriter w = Writer(out, capacity);
    if (note == NULL)return UMI_STATUS_INVALID_ARGUMENT;
    Add( & w, "umicom.creative.note/1\n");
    Hex( & w, note -> id, sizeof(note -> id));
    Unsigned( & w, note -> startTick);
    Unsigned( & w, note -> durationTicks);
    Unsigned( & w, note -> pitch);
    Unsigned( & w, note -> velocity);
    return EndWrite( & w);
}

UmiStatus UmiCreativeWireNoteRead(const char * wire, UmiCreativeNote * out)
{
    if (out == NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof( * out));
    WireReader r = Reader(wire);
    Header( & r, "umicom.creative.note/1");
    Text( & r, out -> id, sizeof(out -> id));
    out -> startTick = (uint32_t)Number( & r, UINT32_MAX);
    out -> durationTicks = (uint32_t)Number( & r, UINT32_MAX);
    out -> pitch = (uint8_t)Number( & r, 127U);
    out -> velocity = (uint8_t)Number( & r, 127U);
    UmiStatus status = EndRead( & r);
    if (status!= UMI_STATUS_OK)memset(out, 0, sizeof( * out));
    return status;
}

