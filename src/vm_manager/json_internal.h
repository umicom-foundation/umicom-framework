/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Strict QMP wire reader. The existing language token reader remains unchanged;
 * this protocol boundary requires depth limits, exact primitives, decoded-key
 * uniqueness and complete UTF-8/surrogate validation before command handling. */
#ifndef UMICOM_VM_JSON_INTERNAL_H
#define UMICOM_VM_JSON_INTERNAL_H
#include "umicom/vm_manager/qmp.h"
typedef struct VjToken {
    int type,parent,next;
    size_t start,end;
}
VjToken;
typedef struct VjDocument {
    const unsigned char *bytes;
    size_t length,used,count;
    VjToken tokens[512];
}
VjDocument;
enum {
    VJ_OBJECT=1,VJ_ARRAY,VJ_STRING,VJ_NUMBER,VJ_TRUE,VJ_FALSE,VJ_NULL
};
UmiStatus VjParse(const void *data,size_t size,VjDocument *doc);
int VjGet(const VjDocument *doc,int object,const char *name);
UmiStatus VjString(const VjDocument *doc,int token,char *out,size_t capacity);
UmiStatus VjUnsigned(const VjDocument *doc,int token,uint64_t *out);
int VmUtf8(const char *text,size_t maximum);
#endif
