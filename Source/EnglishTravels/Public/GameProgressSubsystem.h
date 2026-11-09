#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameProgressSubsystem.generated.h"

class UEnglishTravelsSaveGame;

UCLASS()
class ENGLISHTRAVELS_API UGameProgressSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category = "Save")
    bool StartNewGame();

    UFUNCTION(BlueprintCallable, Category = "Save")
    bool ContinueGame();

    UFUNCTION(BlueprintCallable, Category = "Save")
    bool InitializeGameProgress();

    UFUNCTION(BlueprintCallable, Category = "Save")
    bool HasSaveGame() const;

    UFUNCTION(BlueprintCallable, Category = "Save")
    bool SaveProgress();

    UFUNCTION(BlueprintPure, Category = "Save")
    bool IsGameInitialized() const;

    UFUNCTION(BlueprintPure, Category = "Save")
    bool WasLastSaveSuccessful() const;

    UFUNCTION(BlueprintCallable, Category = "Progress")
    bool MarkLetterOpened(FName LetterId, FName LocationId);

    UFUNCTION(BlueprintPure, Category = "Progress")
    bool IsLetterOpened(FName LetterId) const;

    UFUNCTION(BlueprintCallable, Category = "Progress")
    bool MarkLocationCompleted(FName LocationId);

    UFUNCTION(BlueprintPure, Category = "Progress")
    bool IsLocationCompleted(FName LocationId) const;

    UFUNCTION(BlueprintPure, Category = "Progress")
    FName GetActiveLocationId() const;

    UFUNCTION(BlueprintPure, Category = "Progress")
    TArray<FName> GetCompletedLocations() const;

    UFUNCTION(BlueprintCallable, Category = "Progress")
    void SetNotebookNotes(const FString& Notes);

    UFUNCTION(BlueprintPure, Category = "Progress")
    FString GetNotebookNotes() const;

    void AutoSave();

private:
    void ApplySave(UEnglishTravelsSaveGame* Save);

    UPROPERTY()
    TObjectPtr<UEnglishTravelsSaveGame> CurrentSave;

    bool bInitialized = false;
    bool bLastSaveSuccessful = false;
};
