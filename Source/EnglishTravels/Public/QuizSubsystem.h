#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "QuizTypes.h"
#include "QuizSubsystem.generated.h"

UENUM(BlueprintType)
enum class EQuizAnswerResult : uint8
{
    Correct UMETA(DisplayName = "Correct"),
    Incorrect UMETA(DisplayName = "Incorrect"),
    AlreadyFailedThisQuestion UMETA(DisplayName = "Already Failed This Question")
};

UCLASS()
class ENGLISHTRAVELS_API UQuizSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Quiz")
    void StartQuiz();

    UFUNCTION(BlueprintCallable, Category = "Quiz")
    FQuizQuestion GetCurrentQuestion() const;

    UFUNCTION(BlueprintCallable, Category = "Quiz")
    EQuizAnswerResult SubmitAnswer(int32 SelectedAnswerIndex);

    UFUNCTION(BlueprintCallable, Category = "Quiz")
    bool MoveToNextQuestion();

    UFUNCTION(BlueprintCallable, Category = "Quiz")
    bool IsQuizFinished() const;

    UFUNCTION(BlueprintCallable, Category = "Quiz")
    int32 GetCorrectAnswersCount() const;

    UFUNCTION(BlueprintCallable, Category = "Quiz")
    int32 GetMistakesCount() const;

    UFUNCTION(BlueprintCallable, Category = "Quiz")
    int32 GetCurrentQuestionIndex() const;

    UFUNCTION(BlueprintCallable, Category = "Quiz")
    int32 GetTotalQuestionsCount() const;

    //max 1 mistake to pass
    UFUNCTION(BlueprintCallable, Category = "Quiz")
    bool IsQuizPassed() const;

    //player finished quiz?
    UFUNCTION(BlueprintCallable, Category = "Quiz")
    bool HasCompletedQuiz() const;

    //player passed quiz?
    UFUNCTION(BlueprintCallable, Category = "Quiz")
    bool HasPassedQuiz() const;

private:
    UPROPERTY()
    TArray<FQuizQuestion> Questions;

    UPROPERTY()
    int32 CurrentQuestionIndex = 0;

    UPROPERTY()
    int32 CorrectAnswersCount = 0;

    UPROPERTY()
    int32 MistakesCount = 0;

    UPROPERTY()
    bool bCurrentQuestionHadMistake = false;

    UPROPERTY()
    bool bQuizCompleted = false;
};