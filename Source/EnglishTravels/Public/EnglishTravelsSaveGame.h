#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "WordTypes.h"
#include "EnglishTravelsSaveGame.generated.h"

UCLASS()
class ENGLISHTRAVELS_API UEnglishTravelsSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, SaveGame, Category = "Save")
    int32 SaveVersion = 1;

    UPROPERTY(EditAnywhere, SaveGame, Category = "Save")
    FDateTime LastSavedUtc;

    UPROPERTY(EditAnywhere, SaveGame, Category = "Save")
    TMap<FName, FWordProgress> WordProgress;

    UPROPERTY(EditAnywhere, SaveGame, Category = "Save")
    TSet<FName> OpenedLetters;

    UPROPERTY(EditAnywhere, SaveGame, Category = "Save")
    TSet<FName> CompletedLocations;

    UPROPERTY(EditAnywhere, SaveGame, Category = "Save")
    FName ActiveLocationId;

    UPROPERTY(EditAnywhere, SaveGame, Category = "Save")
    FString NotebookNotes;
};
