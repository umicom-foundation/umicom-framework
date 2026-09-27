/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * The VM, image and delivery consumers must share the existing bootstrap file
 * rules. These public entry points deliberately delegate; no new I/O policy. */
#include "internal.h"
#include "umicom/setup_centre/files.h"
UmiStatus UmiSetupFileRead(const char *p,size_t l,char **b,size_t *n,UmiSetupReport *r){
    return ScRead(p,l,b,n,r);
}
UmiStatus UmiSetupFileWriteNew(const char *p,const void *b,size_t n,UmiSetupReport *r){
    return ScWriteNew(p,b,n,r);
}
UmiStatus UmiSetupFileDigest(const char *p,char h[65],uint64_t *n,UmiSetupReport *r){
    return ScDigest(p,h,n,NULL,NULL,r);
}
UmiStatus UmiSetupFileCopyChecked(const char *s,const char *d,const char *h,uint64_t n,UmiSetupReport *r){
    return ScCopy(s,d,h,n,NULL,NULL,r);
}
UmiStatus UmiSetupDirectoryCreate(const char *p,UmiSetupReport *r){
    return ScMakeDirectory(p,r);
}
UmiStatus UmiSetupFileCheck(const char *p,int d,UmiSetupReport *r){
    return ScFileRegular(p,d,r);
}
UmiStatus UmiSetupFileParents(const char *p,const char *n,UmiSetupReport *r){
    return ScParents(p,n,r);
}
UmiStatus UmiSetupPathJoin(const char *p,const char *n,char o[UMI_SETUP_PATH_CAPACITY]){
    return ScJoin(p,n,o);
}
UmiStatus UmiSetupFileReadHeader(const char*p,unsigned char*b,size_t c,size_t*n,UmiSetupReport*r){
    if(!p||!b||!n||!c||c>65536U)return UMI_STATUS_INVALID_ARGUMENT;
    return ScHeaderRead(p,b,c,n,r);
}
UmiStatus UmiSetupNativeProgramCheck(const char *path,UmiSetupReport *report){
    return ScHostProgram(path,report);
}
