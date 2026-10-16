#pragma once

#include "CoreMinimal.h"
#include "QuizTypes.generated.h"

USTRUCT(BlueprintType)
struct ENGLISHTRAVELS_API FQuizQuestion
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quiz")
    FText QuestionText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quiz")
    FText AnswerA;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quiz")
    FText AnswerB;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quiz")
    FText AnswerC;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quiz")
    int32 CorrectAnswerIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quiz")
    FName WordId;
};