#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "VocabularySubsystem.generated.h"

UCLASS()
class ENGLISHTRAVELS_API UVocabularySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Vocabulary")
    bool MarkWordAsLearned(FName WordId);

    UFUNCTION(BlueprintCallable, Category = "Vocabulary")
    bool IsWordLearned(FName WordId) const;

    UFUNCTION(BlueprintCallable, Category = "Vocabulary")
    int32 GetLearnedWordsCount() const;

    UFUNCTION(BlueprintCallable, Category = "Vocabulary")
    int32 GetTotalWordsCount() const;

    UFUNCTION(BlueprintCallable, Category = "Vocabulary")
    void SetTotalWordsCount(int32 NewTotalWordsCount);

private:
    UPROPERTY()
    TSet<FName> LearnedWords;

    UPROPERTY()
    int32 TotalWordsCount = 8;
};