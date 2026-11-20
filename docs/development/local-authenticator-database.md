![Umicom](../../resources/brand/umicom-logo.svg)

# Keep authenticator enrollment and replay checks across restarts

A time-based one-time password changes every 30 seconds. The app remembers
the last time step it accepted so the same code cannot open a second session.
The local authenticator database stores this decision even when the app closes.
On Windows, Data Protection API (DPAPI) protects the authenticator key for the
current Windows user. SQLite stores the encrypted key and the enrollment and
replay revisions in the same database.

## Set up an isolated local account

1. Link the app to `Umicom::security`. Include
   `umicom/security/local_mfa_database.h`.
2. Choose a private directory owned by the Windows user. Pass an absolute
   path inside it to `UmiLocalMfaDatabaseOpen`, together with the same
   application ID used by the password verifier. Open a separate database
   handle per worker or serialize every call on a shared handle.
3. Use `UmiLocalProfileRegister` to create a practice profile.
   Use fictional values for a first exercise and keep the database outside
   source control.
4. Call `UmiLocalMfaDatabasePrepare` with the current password, the app's
   issuer label and an expiry in Unix nanoseconds. The service generates
   a fresh key using the OS random source and returns a URI and revision
   after storing the pending enrollment. Return the URI only to the
   person enrolling and erase the caller's copy with `umi_secret_clear`.
   The URI contains the key. Keep it out of logs, browser URLs and files
   intended for sharing.

## Verify the authenticator and open a session

1. Keep the native database and password store open for the account workflow.
   Import the prepared URI into an authenticator that supports the chosen
   algorithm and code width.
2. Use `UmiLocalMfaDatabaseConfirmEnrollment` with the current password,
   the returned revision and a fresh code. The database checks that the
   enrollment belongs to the same password registration.
   Success activates the exact pending key and consumes the code's time step.
   An expired or superseded enrollment requires a fresh preparation.
3. Wait for a new code and call `UmiLocalMfaDatabaseSignIn` with it and
   the current password. The service establishes a session only after
   the database records the newly accepted time step. A replay attempt
   fails even if the database is closed and reopened.

The compatibility callbacks from `UmiLocalMfaDatabaseBackends` remain
available for established clients. Those clients can explicitly use
`UmiLocalProfileMfaConfirmEnrollment` and `UmiLocalProfileMfaSignIn` with
their existing policy for earlier enrollment records. The native methods
above require a stored password registration binding. A refused native
sign-in should stay refused until the user or administrator resolves it
through the application's account workflow.

Once this app requires MFA, enforce the same requirement at every session
entry point. The host owns rate limits, clock selection and session lifetime.
The current database supports initial activation. An active credential
refuses a second preparation, so possession of the password alone cannot
replace the authenticator. Factor replacement and remote identity providers
require separate workflows. Backing up and restoring database
files together with account state is an administrative operation and can roll
back an already accepted step.

## Keep one-time recovery codes

1. After setup, ask for the current password and a fresh code from the
   authenticator. Pass them to `UmiLocalMfaDatabaseIssueRecovery` using the
   same private database. The service consumes that authenticator step and
   saves eight new recovery digests together. A digest is a one-way value
   computed from a random code. The database keeps the digest for comparison.
2. Show the eight codes once in the authenticated application. Each code
   contains 32 lowercase hexadecimal characters. Allow the user to store the
   codes somewhere they can reach if they lose their authenticator. Clear the
   application's copy with `umi_secret_clear` after display and keep the codes
   out of logs and reports.
3. When the user chooses recovery sign-in, require the current password and
   one unused code. Pass the exact code bytes to
   `UmiLocalMfaDatabaseRecoverySignIn`. The service consumes that code before
   issuing a session. If the database records consumption before another
   failure, use a different unused code on the next attempt.
4. To replace the remaining codes, repeat step 1 with a new authenticator
   code. Successful replacement retires the previous set. The recovery set
   belongs to the password registration that prepared the enrollment. An
   earlier enrollment without that recorded binding can still use ordinary
   authenticator sign-in. Recovery issuance requires a bound enrollment.

Apply the same attempt limits and session lifetime to both sign-in methods.
Like authenticator replay state, restoring a backup can restore code
availability from the time of the backup.

## Reproduce the behaviour

From a Windows build configured with SQLite, build the
`umicom-local-mfa-database-test` target and run the cases matching
`^framework[.]local_profile[.]mfa_database[.]`. The fixture checks encrypted
storage, stale tickets, durable replay refusal and app and profile separation.
The bound enrollment cases also check a password registration recreated
under the same account name before confirmation and after activation.
The recovery cases cover password-first access, one-use codes after reopening,
replacement, transactional failure, concurrent requests and re-created profiles.
If the operating system cannot open the storage provider, preserve the
diagnostic and check the private path and Windows user profile.
