/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_report_view/fixture.h
 *
 * PURPOSE:
 *   Construct owned saved-report fixtures with explicit outcome and ordering differences.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_CTEST_VIEW_FIXTURE_H
#define UMICOM_TEST_CTEST_VIEW_FIXTURE_H
#include "umicom/testing/ctest_report_view.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      exit(1);                                                                                     \
    }                                                                                              \
  } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static inline int Known(const char *name, const char *const *names, size_t count) {
  for (size_t i = 0; i < count; ++i)
    if (!strcmp(name, names[i]))
      return 1;
  return 0;
}
static inline UmiCtestReport *Mixed(void) {
  const char xml[] =
      "<testsuite tests='6' failures='2' disabled='1' skipped='2'>"
      "<testcase name='z.pass' status='run' time='1.2'/>"
      "<testcase name='b.fail' status='fail' time='0.2'><failure message='Failed'/></testcase>"
      "<testcase name='d.timeout' status='fail' time='0.3'><failure message='Timeout'/></testcase>"
      "<testcase name='c.skip' status='notrun' time='0'><skipped "
      "message='SKIP_RETURN_CODE=77'/></testcase>"
      "<testcase name='e.disabled' status='disabled' time='0'/>"
      "<testcase name='a.missing' status='notrun' time='0'><skipped message='Unable to find "
      "executable'/></testcase>"
      "</testsuite>";
  UmiCtestReport *r = NULL;
  OK(UmiCtestReportImport(xml, sizeof xml - 1U, &r));
  return r;
}
static inline char *ManyXml(size_t count) {
  size_t capacity = count * 180U + 100U;
  char *xml = malloc(capacity);
  CHECK(xml);
  int n = snprintf(xml, capacity, "<testsuite tests='%zu'>", count);
  CHECK(n > 0);
  size_t used = (size_t)n;
  for (size_t i = 0; i < count; ++i) {
    n = snprintf(xml + used, capacity - used,
                 "<testcase name='row-%06zu' status='fail' time='%zu'><failure "
                 "message='Failed'/></testcase>",
                 count - i - 1U, i % 3U);
    CHECK(n > 0 && (size_t)n < capacity - used);
    used += (size_t)n;
  }
  memcpy(xml + used, "</testsuite>", 13U);
  return xml;
}
static inline UmiCtestReport *Many(size_t count) {
  char *xml = ManyXml(count);
  UmiCtestReport *r = NULL;
  OK(UmiCtestReportImport(xml, strlen(xml), &r));
  free(xml);
  return r;
}
static inline char *PageText(const UmiCtestReportView *view, size_t offset, size_t limit) {
  size_t required = 0;
  OK(UmiCtestReportViewFormat(view, offset, limit, NULL, 0, &required));
  char *text = malloc(required + 1U);
  CHECK(text);
  OK(UmiCtestReportViewFormat(view, offset, limit, text, required + 1U, &required));
  return text;
}
#endif
