#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WordTypes.h"
#include "WritingSubsystem.generated.h"

UENUM(BlueprintType)
enum class EWritingAnswerResult : uint8
{
    Ignored,
    EmptyAnswer,
    Correct,
    Incorrect
};

UCLASS()
class ENGLISHTRAVELS_API UWritingSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Writing")
    bool StartSessionByCategory(FName Category, int32 MaxWords = 5);

    UFUNCTION(BlueprintCallable, Category = "Writing")
    EWritingAnswerResult SubmitAnswer(const FString& Answer);

    UFUNCTION(BlueprintCallable, Category = "Writing")
    bool SkipCurrentWord();

    UFUNCTION(BlueprintCallable, Category = "Writing")
    bool MoveToNextWord();

    UFUNCTION(BlueprintCallable, Category = "Writing")
    void EndSession();

    UFUNCTION(BlueprintPure, Category = "Writing")
    FWordData GetCurrentWord() const;

    UFUNCTION(BlueprintPure, Category = "Writing")
    bool IsCurrentWordAnswered() const;

    UFUNCTION(BlueprintPure, Category = "Writing")
    bool IsSessionFinished() const;

    UFUNCTION(BlueprintPure, Category = "Writing")
    int32 GetCurrentWordIndex() const;

    UFUNCTION(BlueprintPure, Category = "Writing")
    int32 GetTotalWordsCount() const;

    UFUNCTION(BlueprintPure, Category = "Writing")
    int32 GetCorrectAnswersCount() const;

    UFUNCTION(BlueprintPure, Category = "Writing")
    int32 GetIncorrectAnswersCount() const;

    UFUNCTION(BlueprintPure, Category = "Writing")
    int32 GetSkippedWordsCount() const;

private:
    UPROPERTY()
    TArray<FWordData> Words;

    int32 CurrentWordIndex = 0;
    int32 CorrectAnswersCount = 0;
    int32 IncorrectAnswersCount = 0;
    int32 SkippedWordsCount = 0;
    bool bCurrentWordAnswered = false;
};
