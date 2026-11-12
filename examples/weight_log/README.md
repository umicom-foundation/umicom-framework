![Umicom](../../resources/brand/umicom-logo.svg)

# Build the native weight-log example

This C23 example creates one synthetic observation using Framework's existing structured-form service. It demonstrates exact kilogram input, a calendar date, an explicit mass.g unit and a validated copied result. It does not create a window, download data, persist personal records or recommend a weight target.

## Before you begin

Use a Framework SDK containing umicom/healthcare/weight_log.h and the matching Umicom::application library. A header copied from a newer source tree does not upgrade an older installed library. Configure and build that SDK through your normal qualified workflow first.

From the Framework source root, replace C:/sdk/umicom with your actual SDK prefix:

~~~powershell
cmake -S examples/weight_log -B build/weight-log-consumer -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=C:/sdk/umicom
cmake --build build/weight-log-consumer
ctest --test-dir build/weight-log-consumer --output-on-failure
~~~

Run each command separately and stop on an error. The intended output is:

~~~text
Synthetic observation: 2024-02-29, 80250 g
~~~

The example and new regression cases require execution in your qualification environment; source presence does not prove they passed.

## Follow the ownership

1. Allocate a UmiStructuredForm in caller-owned storage; the schema remains borrowed and immutable.
2. Parse the explicit kilogram text into exact integer grams. No floating-point conversion or implicit unit guessing occurs.
3. Create typed answers and pass the currently reviewed revision to umi_structured_form_set.
4. Call umi_weight_log_read to obtain a validated copy.
5. Release the form storage after all views and pending operations stop using it.

The service has an encoding maximum of 1,000,000,000 grams. This is a software capacity, not a recommended human weight. It supplies no health classification, calorie calculation or treatment guidance.

## Continue to other clients

Use UmiStructuredFormGtk4New for an existing native form host, or umi_web_structured_form_render for an authorized web host. Both continue to use the same schema. The web host still owns sessions, CSRF token generation, authorization and transport. A mobile host needs a separately qualified lifecycle and platform build; linking the shared C service alone is not a finished mobile application.

Saving a form uses the existing structured_form_record service and a host-owned Data Server. Do not confuse the form edit revision with the expected stored record revision. Do not log personal measurements, notes or request bodies as routine diagnostics.

## Recovery

A missing SDK component is a configuration problem: inspect the selected prefix before changing source includes. An unresolved new symbol usually means headers and libraries came from different SDK builds. Invalid dates, wrong units or an incomplete form must be corrected in the draft; a refused read leaves its output unchanged.

For source regressions, select the framework.weight_log.* tests from an appropriately configured Framework build. The synthetic example can be inspected without installing a GUI client.

