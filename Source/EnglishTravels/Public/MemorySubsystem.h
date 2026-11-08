#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MemoryTypes.h"
#include "WordTypes.h"
#include "MemorySubsystem.generated.h"

UCLASS()
class ENGLISHTRAVELS_API UMemorySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Memory")
    bool StartSession(int32 MaxPairs = 6);

    UFUNCTION(BlueprintCallable, Category = "Memory")
    bool StartSessionByCategory(FName Category, int32 MaxPairs = 6);

    UFUNCTION(BlueprintCallable, Category = "Memory")
    EMemorySelectionResult SelectCard(int32 CardIndex);

    UFUNCTION(BlueprintCallable, Category = "Memory")
    bool ResolveMismatch();

    UFUNCTION(BlueprintCallable, Category = "Memory")
    void EndSession();

    UFUNCTION(BlueprintPure, Category = "Memory")
    TArray<FMemoryCard> GetCards() const;

    UFUNCTION(BlueprintPure, Category = "Memory")
    FMemoryCard GetCard(int32 CardIndex) const;

    UFUNCTION(BlueprintPure, Category = "Memory")
    bool IsBoardLocked() const;

    UFUNCTION(BlueprintPure, Category = "Memory")
    bool IsSessionFinished() const;

    UFUNCTION(BlueprintPure, Category = "Memory")
    int32 GetMatchedPairsCount() const;

    UFUNCTION(BlueprintPure, Category = "Memory")
    int32 GetTotalPairsCount() const;

private:
    bool StartSessionFromWords(const TArray<FWordData>& Words, int32 MaxPairs);

    UPROPERTY()
    TArray<FMemoryCard> Cards;

    int32 FirstCardIndex = INDEX_NONE;
    int32 SecondCardIndex = INDEX_NONE;
    int32 MatchedPairsCount = 0;
    bool bBoardLocked = false;
};
