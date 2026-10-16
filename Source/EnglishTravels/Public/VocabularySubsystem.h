#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WordTypes.h"
#include "VocabularySubsystem.generated.h"

class UDataTable;

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
    bool AreAllWordsLearned() const;

    UFUNCTION(BlueprintCallable, Category = "Vocabulary")
    void SetTotalWordsCount(int32 NewTotalWordsCount);

    UFUNCTION(BlueprintCallable, Category = "Vocabulary")
    void SetWordTable(UDataTable* NewWordTable);

    UFUNCTION(BlueprintCallable, Category = "Vocabulary")
    FWordData GetWordById(FName WordId) const;

    UFUNCTION(BlueprintCallable, Category = "Vocabulary")
    TArray<FWordData> GetWordsBySet(FName SetId) const;

    UFUNCTION(BlueprintCallable, Category = "Vocabulary")
    TArray<FWordData> GetWordsByCategory(FName Category) const;

    UFUNCTION(BlueprintCallable, Category = "Vocabulary")
    void MarkWordAsDiscovered(FName WordId);

    UFUNCTION(BlueprintCallable, Category = "Vocabulary")
    void RegisterCorrectAnswer(FName WordId);

    UFUNCTION(BlueprintCallable, Category = "Vocabulary")
    void RegisterMistake(FName WordId);

    UFUNCTION(BlueprintCallable, Category = "Vocabulary")
    FWordProgress GetWordProgress(FName WordId) const;

private:
    UPROPERTY()
    TSet<FName> LearnedWords;

    UPROPERTY()
    int32 TotalWordsCount = 3;

    UPROPERTY()
    UDataTable* WordTable = nullptr;

    UPROPERTY()
    TMap<FName, FWordProgress> WordProgress;
};