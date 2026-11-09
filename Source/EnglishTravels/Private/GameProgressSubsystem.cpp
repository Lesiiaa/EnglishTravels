#include "GameProgressSubsystem.h"
#include "DialogueSubsystem.h"
#include "EnglishTravelsSaveGame.h"
#include "Engine/GameInstance.h"
#include "FlashcardSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "MemorySubsystem.h"
#include "QuizSubsystem.h"
#include "VocabularySubsystem.h"

namespace
{
    const FString SaveSlotName = TEXT("EnglishTravelsProgress");
    constexpr int32 SaveUserIndex = 0;
    constexpr int32 SupportedSaveVersion = 1;
}

void UGameProgressSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    Collection.InitializeDependency<UVocabularySubsystem>();
    Collection.InitializeDependency<UQuizSubsystem>();
    Collection.InitializeDependency<UFlashcardSubsystem>();
    Collection.InitializeDependency<UMemorySubsystem>();
    Collection.InitializeDependency<UDialogueSubsystem>();
}

bool UGameProgressSubsystem::StartNewGame()
{
    //replace the save before resetting the current session
    UEnglishTravelsSaveGame* Save = Cast<UEnglishTravelsSaveGame>(
        UGameplayStatics::CreateSaveGameObject(UEnglishTravelsSaveGame::StaticClass()));

    if (!Save)
    {
        bLastSaveSuccessful = false;
        return false;
    }

    Save->LastSavedUtc = FDateTime::UtcNow();
    bLastSaveSuccessful = UGameplayStatics::SaveGameToSlot(Save, SaveSlotName, SaveUserIndex);

    if (!bLastSaveSuccessful)
    {
        UE_LOG(LogTemp, Error, TEXT("Could not create a new EnglishTravels save."));
        return false;
    }

    ApplySave(Save);
    return true;
}

bool UGameProgressSubsystem::ContinueGame()
{
    //load game progress only from a supported save version
    UEnglishTravelsSaveGame* Save = Cast<UEnglishTravelsSaveGame>(
        UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveUserIndex));

    if (!Save || Save->SaveVersion != SupportedSaveVersion)
    {
        UE_LOG(LogTemp, Warning, TEXT("EnglishTravels save is missing, unreadable, or unsupported."));
        return false;
    }

    ApplySave(Save);
    bLastSaveSuccessful = true;
    return true;
}

bool UGameProgressSubsystem::InitializeGameProgress()
{
    if (bInitialized)
    {
        return true;
    }

    return UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveUserIndex)
        ? ContinueGame() : StartNewGame();
}

bool UGameProgressSubsystem::HasSaveGame() const
{
    if (!UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveUserIndex))
    {
        return false;
    }

    const UEnglishTravelsSaveGame* Save = Cast<UEnglishTravelsSaveGame>(
        UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveUserIndex));
    return Save && Save->SaveVersion == SupportedSaveVersion;
}

void UGameProgressSubsystem::ApplySave(UEnglishTravelsSaveGame* Save)
{
    //restore persistent progress and clear temporary exercise sessions
    bInitialized = false;
    CurrentSave = Save;
    GetGameInstance()->GetSubsystem<UVocabularySubsystem>()->RestoreProgress(Save->WordProgress);
    GetGameInstance()->GetSubsystem<UQuizSubsystem>()->ResetSession();
    GetGameInstance()->GetSubsystem<UFlashcardSubsystem>()->EndSession();
    GetGameInstance()->GetSubsystem<UMemorySubsystem>()->EndSession();
    GetGameInstance()->GetSubsystem<UDialogueSubsystem>()->EndDialogue();
    bInitialized = true;
}

bool UGameProgressSubsystem::SaveProgress()
{
    if (!bInitialized || !CurrentSave)
    {
        return false;
    }

    //save vocabulary progress together with story state and notebook notes
    CurrentSave->WordProgress = GetGameInstance()->GetSubsystem<UVocabularySubsystem>()->ExportProgress();
    CurrentSave->LastSavedUtc = FDateTime::UtcNow();
    bLastSaveSuccessful = UGameplayStatics::SaveGameToSlot(CurrentSave, SaveSlotName, SaveUserIndex);

    if (!bLastSaveSuccessful)
    {
        UE_LOG(LogTemp, Error, TEXT("Could not save EnglishTravels progress."));
    }

    return bLastSaveSuccessful;
}

void UGameProgressSubsystem::AutoSave()
{
    if (bInitialized)
    {
        SaveProgress();
    }
}

bool UGameProgressSubsystem::IsGameInitialized() const
{
    return bInitialized;
}

bool UGameProgressSubsystem::WasLastSaveSuccessful() const
{
    return bLastSaveSuccessful;
}

bool UGameProgressSubsystem::MarkLetterOpened(FName LetterId, FName LocationId)
{
    if (!bInitialized || !CurrentSave || LetterId.IsNone() || LocationId.IsNone())
    {
        return false;
    }

    const bool bChanged = !CurrentSave->OpenedLetters.Contains(LetterId);
    CurrentSave->OpenedLetters.Add(LetterId);

    if (!CurrentSave->CompletedLocations.Contains(LocationId))
    {
        CurrentSave->ActiveLocationId = LocationId;
    }

    AutoSave();
    return bChanged;
}

bool UGameProgressSubsystem::IsLetterOpened(FName LetterId) const
{
    return CurrentSave && CurrentSave->OpenedLetters.Contains(LetterId);
}

bool UGameProgressSubsystem::MarkLocationCompleted(FName LocationId)
{
    if (!bInitialized || !CurrentSave || LocationId.IsNone())
    {
        return false;
    }

    const bool bChanged = !CurrentSave->CompletedLocations.Contains(LocationId);
    CurrentSave->CompletedLocations.Add(LocationId);

    if (CurrentSave->ActiveLocationId == LocationId)
    {
        CurrentSave->ActiveLocationId = NAME_None;
    }

    AutoSave();
    return bChanged;
}

bool UGameProgressSubsystem::IsLocationCompleted(FName LocationId) const
{
    return CurrentSave && CurrentSave->CompletedLocations.Contains(LocationId);
}

FName UGameProgressSubsystem::GetActiveLocationId() const
{
    return CurrentSave ? CurrentSave->ActiveLocationId : NAME_None;
}

TArray<FName> UGameProgressSubsystem::GetCompletedLocations() const
{
    return CurrentSave ? CurrentSave->CompletedLocations.Array() : TArray<FName>();
}

void UGameProgressSubsystem::SetNotebookNotes(const FString& Notes)
{
    if (bInitialized && CurrentSave && CurrentSave->NotebookNotes != Notes)
    {
        CurrentSave->NotebookNotes = Notes;
        AutoSave();
    }
}

FString UGameProgressSubsystem::GetNotebookNotes() const
{
    return CurrentSave ? CurrentSave->NotebookNotes : FString();
}
