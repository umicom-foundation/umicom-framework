/* Umicom Framework | Security governance capabilities v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_SECURITY_CATALOGUE_H
#define UMICOM_SECURITY_CATALOGUE_H
#include <stddef.h>
size_t umi_security_catalogue_count(void);
const char *umi_security_catalogue_at(size_t index);
#endif
