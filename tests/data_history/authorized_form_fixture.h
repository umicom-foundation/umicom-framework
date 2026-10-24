/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Combine fictional typed answers with the real authority and storage adapter. */
#ifndef UMICOM_AUTHORIZED_FORM_FIXTURE_H
#define UMICOM_AUTHORIZED_FORM_FIXTURE_H
#include "../application_experience/structured_form_fixture.h"
#undef CHECK
#include "../security/record_access_fixture.h"
#include "umicom/data/authorized_form_records.h"
typedef struct AuthorizedFormFixture {
  AccessFixture access;
  FormFixture *values;
  UmiAuthorizedFormRecord *loaded;
  UmiAuthorizedRecordRevision saved;
} AuthorizedFormFixture;
static inline UmiStatus authorized_form_new(AuthorizedFormFixture *f, const char *backend) {
  memset(f, 0, sizeof *f);
  UmiStatus s = access_new(&f->access, backend);
  if (s == UMI_STATUS_OK)
    s = access_allow(&f->access);
  f->values = form_fixture_new();
  f->loaded = malloc(sizeof *f->loaded);
  if (!f->values || !f->loaded)
    return UMI_STATUS_OUT_OF_MEMORY;
  UmiFormAnswer answer = form_mass(3.5);
  if (s == UMI_STATUS_OK)
    s = umi_structured_form_set(&f->values->schema, &f->values->before, 1, "parcel.mass", &answer);
  f->access.request.operation = UMI_RECORD_ACCESS_CREATE;
  return s;
}
static inline UmiStatus authorized_form_save(AuthorizedFormFixture *f, uint64_t expected,
                                             UmiAuthorizedRecordRevision *out) {
  f->access.request.operation = expected ? UMI_RECORD_ACCESS_AMEND : UMI_RECORD_ACCESS_CREATE;
  return umi_authorized_form_record_save(f->access.records, &f->access.request, &f->values->schema,
                                         &f->values->before, "Review fictional form", 100, expected,
                                         out);
}
static inline void authorized_form_free(AuthorizedFormFixture *f) {
  access_free(&f->access);
  free(f->loaded);
  free(f->values);
}
#endif
