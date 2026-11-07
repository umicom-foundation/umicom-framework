# Choose where banking simulation data is stored

The banking operations launcher uses an explicit local database filename. It does not choose an AppData location or create directories automatically. The operations are a local simulation; test maker, checker and operator identities do not provide production authentication.

1. Create a folder for your practice data.
2. In the launcher, use Browse existing for a previous simulation database, or Choose new location for a proposed filename. You may also type an absolute path.
3. Press Banking operations to open or create that database. Selection alone performs no storage operation.
4. If the file cannot open, check its parent folder, access permissions and format. Correct the path and retry.
5. Close the operational window before opening another location. While it is open, the launcher presents that same window and keeps its original database.

Older databases remain where they were saved. Select them explicitly if you want to continue that simulation. No automatic migration, copy or deletion is performed. The path is kept only for the launcher's lifetime and is not saved as a preference.

## Embed the launcher

Use UmiGtk4BankOperationsLauncherCreate from umicom/ui/gtk4/bank_operations.h and parent the returned widget into a GTK window. To seed a location that the user already selected for a project, call UmiGtk4BankOperationsLauncherSetPath with the launcher and a UTF-8 absolute filename. An empty string clears the field. Invalid input leaves the field unchanged. Call both functions on the GTK thread.

The shared path picker owns asynchronous chooser requests. Its cancellation and weak-reference checks prevent late results from changing an entry after its window has closed. The banking launcher copies the chosen filename before handing it to UmiBankOperationsOpenSqlite. The banking service and Data Server own database validation and persistence.

## Regression coverage

The native command fixture covers empty selection, absolute Unicode paths, invalid seed rejection, no open on selection alone, failed-open retry, preserving an already-open location, review/submit and close/reopen. It substitutes memory storage only at the database-open boundary. Run the separate storage service suite to qualify durability and restart behaviour.

These checks complement manual chooser cancellation and real-file reopen checks. A passing isolated native test does not establish that the user's installed GTK runtime or filesystem works correctly.
