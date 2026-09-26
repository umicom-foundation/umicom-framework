/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Checked text encoders keep launcher arguments out of a shell. The desktop
 * format has two escape layers: key-value decoding, then Exec token decoding.
 * See the freedesktop Desktop Entry Specification, section The Exec key. */
#include "internal.h"
#include <string.h>
void UmiNativeTextInit(UmiNativeText *text, char *buffer, size_t capacity)
{
    *text = (UmiNativeText){buffer, capacity, 0U, UMI_STATUS_OK};
    if (buffer == NULL || capacity == 0U) text->status = UMI_STATUS_INVALID_ARGUMENT;
    else buffer[0] = '\0';
}
void UmiNativeTextBytes(UmiNativeText *text, const void *bytes, size_t length)
{
    if (text->status != UMI_STATUS_OK) return;
    if (length >= text->capacity - text->length) { text->status = UMI_STATUS_CAPACITY_EXCEEDED; return; }
    if (length != 0U) memcpy(text->data + text->length, bytes, length);
    text->length += length; text->data[text->length] = '\0';
}
void UmiNativeTextString(UmiNativeText *text, const char *value)
{
    UmiNativeTextBytes(text, value, strlen(value));
}
void UmiNativeTextDecimal(UmiNativeText *text, uint64_t value)
{
    char digits[20]; size_t count = 0U;
    do { digits[count++] = (char)('0' + value % 10U); value /= 10U; } while (value != 0U);
    while (count != 0U) UmiNativeTextBytes(text, &digits[--count], 1U);
}
void UmiNativeTextJson(UmiNativeText *text, const char *value)
{
    static const char HEX[] = "0123456789abcdef";
    UmiNativeTextString(text, "\"");
    for (const unsigned char *p = (const unsigned char *)value; *p != 0U; ++p) {
        if (*p == '\\' || *p == '"') UmiNativeTextString(text, "\\");
        if (*p < 32U) {
            char escaped[6] = {'\\','u','0','0',HEX[*p>>4U],HEX[*p&15U]};
            UmiNativeTextBytes(text, escaped, sizeof escaped);
        } else UmiNativeTextBytes(text, p, 1U);
    }
    UmiNativeTextString(text, "\"");
}
void UmiNativeTextEntryValue(UmiNativeText *text, const char *value)
{
    for (const char *p = value; *p != '\0'; ++p) {
        if (*p == '\\') UmiNativeTextString(text, "\\");
        UmiNativeTextBytes(text, p, 1U);
    }
}
void UmiNativeTextEntryArgument(UmiNativeText *text, const char *value)
{
    UmiNativeTextString(text, "\"");
    for (const char *p = value; *p != '\0'; ++p) {
        if (*p == '\\') UmiNativeTextString(text, "\\\\\\\\");
        else {
            if (*p == '"' || *p == '$' || *p == '`') UmiNativeTextString(text, "\\\\");
            UmiNativeTextBytes(text, p, 1U);
        }
    }
    UmiNativeTextString(text, "\"");
}
UmiStatus UmiNativeTextFinish(UmiNativeText *text, size_t *outLength)
{
    if (outLength != NULL) *outLength = text->status == UMI_STATUS_OK ? text->length : 0U;
    if (text->status != UMI_STATUS_OK && text->data != NULL && text->capacity != 0U) text->data[0]='\0';
    return text->status;
}
int UmiNativeUtf8(const unsigned char *bytes, size_t length)
{
    for (size_t i=0U; i<length;) {
        uint32_t c=bytes[i++], minimum=0U; size_t rest=0U;
        if (c < 0x80U) continue;
        if (c>=0xc2U && c<=0xdfU) { rest=1U; minimum=0x80U; c&=0x1fU; }
        else if (c>=0xe0U && c<=0xefU) { rest=2U; minimum=0x800U; c&=0x0fU; }
        else if (c>=0xf0U && c<=0xf4U) { rest=3U; minimum=0x10000U; c&=7U; }
        else return 0;
        if (rest>length-i) return 0;
        while (rest-- != 0U) { unsigned char next=bytes[i++]; if ((next&0xc0U)!=0x80U) return 0; c=(c<<6U)|(next&0x3fU); }
        if (c<minimum || c>0x10ffffU || (c>=0xd800U && c<=0xdfffU)) return 0;
    }
    return 1;
}
UmiStatus UmiNativeStageValidatePath(const char *path, int executableToken)
{
    if (path == NULL || path[0]!='/') return UMI_STATUS_INVALID_ARGUMENT;
    size_t length=0U;
    while (length<UMI_NATIVE_STAGE_PATH_CAPACITY && path[length]!='\0') ++length;
    if (length>=UMI_NATIVE_STAGE_PATH_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length<2U || path[length-1U]=='/' || (executableToken && path[length-1U]==' ') || !UmiNativeUtf8((const unsigned char *)path,length)) return UMI_STATUS_INVALID_ARGUMENT;
    size_t start=1U;
    for (size_t i=0U; i<=length; ++i) {
        unsigned char c=(unsigned char)path[i];
        if (i<length && (c<32U || c==127U || c=='%' || (executableToken && c=='='))) return UMI_STATUS_INVALID_ARGUMENT;
        if (i>0U && (c=='/' || c==0U)) {
            size_t n=i-start;
            if (n==0U || n>255U || (n==1U && path[start]=='.') || (n==2U && path[start]=='.' && path[start+1U]=='.'))
                return UMI_STATUS_INVALID_ARGUMENT;
            start=i+1U;
        }
    }
    return UMI_STATUS_OK;
}
int UmiNativeHashValid(const char *hash)
{
    for (size_t i=0U; i<64U; ++i) if (!((hash[i]>='0' && hash[i]<='9') || (hash[i]>='a' && hash[i]<='f'))) return 0;
    return hash[64]=='\0';
}
UmiStatus UmiNativeManifestValidate(const UmiNativeStageManifest *m)
{
    if (m == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (UmiNativeStageValidatePath(m->root,0)!=UMI_STATUS_OK || UmiNativeStageValidatePath(m->desk.path,0)!=UMI_STATUS_OK
        || UmiNativeStageValidatePath(m->launcher.path,1)!=UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    const UmiNativeStageBinary *values[2]={&m->desk,&m->launcher};
    for (size_t i=0U; i<2U; ++i) {
        if (values[i]->bytes<64U || values[i]->bytes>UMI_NATIVE_STAGE_BINARY_LIMIT || !UmiNativeHashValid(values[i]->sha256)
            || (values[i]->machine!=62U && values[i]->machine!=183U && values[i]->machine!=243U)) return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (m->desk.machine != m->launcher.machine || !UmiNativeHashValid(m->iconHash) || !UmiNativeHashValid(m->entryHash))
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
