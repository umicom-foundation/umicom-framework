# Using an identity directory safely

An identity directory is a table of public profiles: stable identifiers, display names, organisation names and enablement state. It does not verify a password or issue a login session. Applications use it as one input to a protected workflow.

## Create and populate the table

1. Include `umicom/security/identity_directory.h` and link `Umicom::security`.
2. Zero-initialise `UmiSecurityIdentityDirectory`. Use allocated storage when a large fixed table is unsuitable for the thread stack.
3. Prepare a `UmiSecurityIdentityProfile`. Identity ID and display name must be non-empty C strings. Organisation can be empty. Each string must end with a zero byte inside its array.
4. Choose a documented `UmiIdentityKind` and call `umi_security_identity_directory_add`.
5. Check the returned status before using the result. A successful addition sets the row revision to one and advances the directory revision.

## Read and change a profile

`umi_security_identity_directory_find` returns a borrowed pointer. The directory owns that storage. Do not free the pointer, and do not keep it after the directory is destroyed. Re-read it after edits when deciding whether an identity is enabled. A missing or invalid lookup returns NULL.

The directory is caller-owned and has no internal lock. Serialize reads and writes through the application's existing service or lock. A pointer being readable does not mean another thread cannot change its contents.

`umi_security_identity_directory_set_enabled` changes enablement and advances both revisions. A repeated assignment retains the existing contract: it still records an edit. If either revision has reached `UINT64_MAX`, the operation returns `UMI_STATUS_CAPACITY_EXCEEDED` and leaves all bytes unchanged.

## Handle rejected input

The used row count is checked before indexing. Every used profile is validated before lookup, so a valid early row does not conceal damaged later metadata. A malformed count or unterminated field returns INVALID_ARGUMENT for mutations. The lookup API has no status output and returns NULL instead. Full capacity and revision exhaustion return CAPACITY_EXCEEDED. A duplicate valid ID returns ALREADY_EXISTS; a missing valid ID during enablement returns NOT_FOUND.

These checks do not make an invalid address safe: the caller must provide live storage of the declared type. A lookup string must remain readable through its terminator, or for the full identifier capacity when intentionally testing a missing terminator.

## Connect to an application

A Bank transfer, Trader order or Studio extension operation must make its permission decision at the operation boundary. Validate the current session and current identity, then apply the permission policy. Hiding a button is presentation, not enforcement. The directory alone is neither an authentication provider nor a policy engine.

## Regression coverage

The cases in `tests/security/test_identity_directory_boundaries.c` cover malformed and maximum-length fields, corrupt counts and later rows, lookup bounds, revision exhaustion, full capacity, repeated enablement and null arguments. They compare byte snapshots after refused mutations. Run the registered `framework.security.identity.directory.*` tests with the matching built Framework revision; source inspection alone does not establish a passing test result.
