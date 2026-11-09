# Save and smart review integration

## Data ownership

`VocabularySubsystem` owns discovered words, knowledge scores, answer statistics,
and review dates. `GameProgressSubsystem` owns opened letters, completed locations,
the active task location, and notebook notes. It writes a single local save slot:
`EnglishTravelsProgress`, user index `0`.

The saved data contains identifiers, counters, and UTC timestamps, not widget or
actor references. Word definitions, categories, and images remain in `DT_Words`.
Keep existing `WordId`, letter IDs, and location IDs stable after distributing a save.

`Shop` is the current story location identifier, matching the existing story quiz.
Do not mark a location complete when leaving it. Discovered words remain available
even when the location's task is incomplete.

## Initial setup before the main menu exists

In `BP_FirstPersonPlayerController` at the beginning of `BeginPlay`:

1. Get `GameProgressSubsystem`.
2. Call `Initialize Game Progress` and branch on its return value.
3. On success, continue the existing HUD, input, and DataTable setup.
4. On failure, show an error; do not automatically replace the save.

This loads an existing save or creates an empty one when no file exists. It does
nothing on subsequent map changes within the same GameInstance. It must run before
word discovery or quiz interactions. Keep the existing `Set Word Table` calls for
VocabularySubsystem and QuizSubsystem. Loading progress does not initialize tables.

When the main menu is added, menu buttons explicitly choose New Game or Continue.
The controller's initialization call can remain as a fallback: it does not reset an
already initialized game. Do not call `Start New Game` from a level's `BeginPlay`.

## Main menu

### Continue availability

On menu construction, call `Has Save Game` and feed its result into the Continue
button's `Set Is Enabled`. This checks the save class and supported version, not
just the presence of a file. Do not call it every frame.

### New Game

`On Clicked -> Start New Game -> Branch -> True: Open Level (HUB)`

This intentionally replaces the existing save and clears word progress, letters,
completed locations, the active task, notes, and temporary exercise/dialogue state.
Ask the player to confirm replacement if a save already exists. On failure, remain
in the menu and report that the new save could not be written.

On either successful menu action, remove the menu widget before travelling. In
the gameplay controller restore `Game Only` input and hide the cursor on entry,
so the new HUB session starts in exploration mode rather than menu input mode.

### Continue

`On Clicked -> Continue Game -> Branch -> True: Open Level (HUB)`

Loading resets temporary exercises/dialogues and restores persistent progress.
The C++ function restores data only; `Open Level` is the Blueprint step that always
sends the player to the HUB. There is no saved actor position or current map.
Use the actual HUB map asset, not the `Shop` location identifier, for `Open Level`.

Until a HUB map exists, a temporary test map may be used for integration. Replace
that map reference in both menu buttons once the HUB is created.

## Letters and tasks

On interaction with a letter, call `Mark Letter Opened`:

- `Letter Id`: for example `ShopLetter`.
- `Location Id`: `Shop`.

Show the letter independently of the returned bool. That bool indicates a newly
opened letter; the player may still reread an already opened one. The call saves
the letter and activates the task if its location has not been completed.

On reconstructing the HUB UI/world:

- `Is Letter Opened(ShopLetter)` restores the read state.
- `Get Active Location Id` restores an unfinished task and its destination.
- `Is Location Completed(Shop)` restores completion messages and task state.

The current story quiz automatically calls `Mark Location Completed(Shop)` when
finished successfully. Training quizzes, flashcards, Memory, failed quizzes, and
early returns do not complete locations. For future story locations, call
`Mark Location Completed` only after their actual completion condition is met.

Before changing maps or quitting through a UI button, call `Save Progress` and
check the result. On an early return, save and travel without calling
`Mark Location Completed` or clearing the active task.

## Vocabulary in a location

Existing calls to `Mark Word As Learned` and `Mark Word As Discovered` now refer to
the same discovered state. Calling discovery again does not add points or reset a
review date. Keep vocabulary objects interactable after loading; they may display
their text again without increasing the discovery count.

The old `Get Learned Words Count` counts all discovered words. For location gates
and location progress UI, use:

- `Get Discovered Words Count In Set(Shop)`.
- `Are All Words In Set Discovered(Shop)`.
- `Get Words By Set(Shop) -> Length` for the total, rather than a fixed value of 3.

Do not reset discovery on entering/leaving a level. This allows discovering part
of a location, returning to the HUB, practicing those words, quitting, and later
continuing the same unfinished task.

## Smart review rules

This is a simple project-specific schedule, not an implementation of Anki's
algorithm. Every due comparison uses `FDateTime::UtcNow()` and actual elapsed time.
Dates persist through process restarts. There is no in-game-day clock or reset at
midnight. Local machine time is the time source.

- A newly discovered word is immediately due.
- Only the smart review mode changes review dates.
- A remembered answer adds 1 knowledge point and schedules the next review using
  the score after the answer: 0-3 = 1 day, 4-6 = 3 days, 7-9 = 7 days,
  10-11 = 14 days, 12 = 30 days.
- A forgotten answer subtracts 1 point, resets the successful-review streak,
  and makes the word due again in 10 minutes.
- Knowledge scores remain within 0-12. Revealing the translation alone changes
  neither points nor dates.
- Quiz and ordinary flashcard practice still update score and answer counters,
  but do not postpone a scheduled review. Memory does not alter word statistics.
- Smart sessions contain up to 10 due words, preferring lower scores, then older
  due dates, with a stable WordId tie-break. Remaining due words can be practiced
  in another smart session.

## Smart review button in WBP_Notebook

Use the existing smart review button, bypassing category selection:

`On Clicked -> Get FlashcardSubsystem -> Start Smart Review(Max Cards: 10)`

Branch on the result:

- True: create `WBP_Flashcards` with `Owning Player = Get Owning Player` and
  `Notebook Ref = Self`, then `Add to Viewport` with ZOrder 10.
- False: keep the notebook open. Show a message such as "Brak slow do powtorki
  w tej chwili". An empty result also occurs before any words are discovered.

Do not restart a session in `WBP_Flashcards -> Construct`; it should only call
`ShowCurrentCard`. Existing Reveal, Remember, Don't Remember, summary, and return
logic are reused. `Submit Answer` determines whether to record ordinary practice
or a scheduled review internally; do not add duplicate scoring in Blueprint.

Optionally use `Is Smart Review Session` to choose the heading "Madra powtorka"
instead of "Fiszki". `Get Due Review Count` can update a notebook counter on open
or after practice, rather than on Tick.

## Autosave and errors

After initialization, word discovery, quiz/flashcard answers, scheduled reviews,
letter reads, task completion, and changes to notebook notes save immediately.
No extra save call is necessary in their Blueprint handlers. `Save Progress`
remains available for travel, quitting, or a manual save button.

`Was Last Save Successful` exposes write failures. Save failures are logged and
the in-memory session remains available for retrying the save. Never replace a
failed Continue with an automatic New Game. Future incompatible save versions
are rejected rather than silently imported.

Optional notes can use `Get Notebook Notes` when opening the notes page and
`Set Notebook Notes` on editing completion or closing, not on every typed letter.

## Integration checks in the editor and packaged build

1. Start a new game: no discovered words, opened letters, completed locations,
   notes, or active tasks. Do not run this against a save you want to keep.
2. Read ShopLetter and discover only part of Shop. Return without passing the
   story quiz. Continue after restarting: spawn in HUB, task still active,
   Shop incomplete, discovered words still in the notebook.
3. Revisit discovered words: their count and due date do not reset.
4. Complete a smart review successfully: score rises by 1 and the due count
   decreases. Continue after restarting: dates, score, and counters are retained.
5. Forget a due word: score falls by 1, next due is about 10 minutes later.
6. Ordinary practice does not move NextReviewUtc; Memory changes no word stats.
7. Pass the story quiz: Shop remains completed after Continue. A subsequent
   training quiz does not complete another location or erase Shop completion.
8. New Game after a previous session clears persistent progress and temporary
   quizzes, flashcards, Memory, and dialogue state. Static DataTables remain set.
9. Test Continue with no file or an unreadable save: remain in the menu, without
   replacing the file or opening a partially restored game.

Not saved: current map/position, current dialogue line, partially completed quiz,
flashcard index, or Memory card arrangement. Their already recorded learning
results are saved, and Continue always starts a fresh session from the HUB.
