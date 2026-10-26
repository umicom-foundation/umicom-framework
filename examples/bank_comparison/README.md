# Compare bank imports with the installed C23 SDK

This example uses invented transactions in memory. It compares exact identifiers,
retains pending/booked distinctions and exports a private review as CSV text.
It does not contact a bank, move money, create files or update a ledger.

1. Install a Framework SDK containing the `bank_feed` component and its dependencies.
2. Configure this example with `UMICOM_SDK_PREFIX` set to that absolute installation
   directory, such as `C:/umicom/installed/applications`.
3. Build `bank-comparison-consumer` and run CTest in its build directory.
4. Run `bank-comparison-consumer compare`. It should report:
   `Exact amount stayed equal; spelling, status and mutability changes were retained.`
   The CSV follows, with one before and one after observation.
5. Run `bank-comparison-consumer missing-id`. It should report:
   `Missing transaction IDs remained separate observations.`
   Equal descriptions and amounts cannot establish a transaction identity.

The program releases both imports before reading the comparison and releases the
comparison before reading its CSV export. Each result owns its retained data.

If package configuration, a header or a symbol is missing, check the chosen SDK
installation. Do not add source-tree include paths or fallback libraries: this
example is intended to qualify the installed public contract. Its two synthetic
scenarios do not prove native UI behavior, provider authenticity or financial
reconciliation of real statements.
