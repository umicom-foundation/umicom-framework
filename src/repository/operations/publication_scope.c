/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/operations/publication_scope.c
 *
 * PURPOSE:
 *   Validate explicit top-level checkout exclusions before publication discovery or Git operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "publication_internal.h"
#include <string.h>
static int Alphanumeric(unsigned char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}
UmiStatus umi_repository_publication_exclusions_validate(const char *const *names, size_t count) {
  if (count > UMI_REPOSITORY_PUBLICATION_EXCLUSION_CAPACITY || (count != 0U && names == NULL))
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t total = 0U;
  for (size_t i = 0U; i < count; ++i) {
    if (names[i] == NULL || !Alphanumeric((unsigned char)names[i][0]))
      return UMI_STATUS_INVALID_ARGUMENT;
    size_t n = 0U;
    for (; n < 64U && names[i][n]; ++n) {
      unsigned char c = (unsigned char)names[i][n];
      if (!Alphanumeric(c) && c != '.' && c != '_' && c != '-')
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (n == 64U || names[i][n - 1U] == '.' || total > 256U - (n + 1U))
      return UMI_STATUS_INVALID_ARGUMENT;
    total += n + 1U;
    for (size_t j = 0U; j < i; ++j) {
      /* Names match exactly during discovery; path equality additionally
       * rejects case aliases on hosts whose path policy ignores case. */
      if (umi_path_equal(names[i], names[j]))
        return UMI_STATUS_INVALID_ARGUMENT;
    }
  }
  return UMI_STATUS_OK;
}
