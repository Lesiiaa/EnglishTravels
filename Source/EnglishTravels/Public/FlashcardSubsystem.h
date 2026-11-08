#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WordTypes.h"
#include "FlashcardSubsystem.generated.h"

UCLASS()
class ENGLISHTRAVELS_API UFlashcardSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Flashcards")
    bool StartSession(FName SetId);

    UFUNCTION(BlueprintCallable, Category = "Flashcards")
    bool StartSessionByCategory(FName Category);

    UFUNCTION(BlueprintPure, Category = "Flashcards")
    FWordData GetCurrentCard() const;

    UFUNCTION(BlueprintCallable, Category = "Flashcards")
    bool RevealCurrentCard();

    UFUNCTION(BlueprintCallable, Category = "Flashcards")
    bool SubmitAnswer(bool bRemembered);

    UFUNCTION(BlueprintPure, Category = "Flashcards")
    bool IsCurrentCardRevealed() const;

    UFUNCTION(BlueprintPure, Category = "Flashcards")
    bool IsSessionFinished() const;

    UFUNCTION(BlueprintPure, Category = "Flashcards")
    int32 GetCurrentCardIndex() const;

    UFUNCTION(BlueprintPure, Category = "Flashcards")
    int32 GetTotalCardsCount() const;

    UFUNCTION(BlueprintPure, Category = "Flashcards")
    int32 GetRememberedCount() const;

    UFUNCTION(BlueprintPure, Category = "Flashcards")
    int32 GetForgottenCount() const;

private:
    bool StartSessionFromWords(const TArray<FWordData>& Words);

    UPROPERTY()
    TArray<FWordData> Cards;

    int32 CurrentCardIndex = 0;
    int32 RememberedCount = 0;
    int32 ForgottenCount = 0;
    bool bCurrentCardRevealed = false;
};
