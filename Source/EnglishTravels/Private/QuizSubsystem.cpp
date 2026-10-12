#include "QuizSubsystem.h"

void UQuizSubsystem::StartQuiz()
{
    Questions.Empty();

    FQuizQuestion Question1;
    Question1.QuestionText = FText::FromString(TEXT("What does \"water\" mean?"));
    Question1.AnswerA = FText::FromString(TEXT("Woda"));
    Question1.AnswerB = FText::FromString(TEXT("Chleb"));
    Question1.AnswerC = FText::FromString(TEXT("Mleko"));
    Question1.CorrectAnswerIndex = 0;
    Questions.Add(Question1);

    FQuizQuestion Question2;
    Question2.QuestionText = FText::FromString(TEXT("What does \"bread\" mean?"));
    Question2.AnswerA = FText::FromString(TEXT("Pomoc"));
    Question2.AnswerB = FText::FromString(TEXT("Chleb"));
    Question2.AnswerC = FText::FromString(TEXT("Gdzie?"));
    Question2.CorrectAnswerIndex = 1;
    Questions.Add(Question2);

    FQuizQuestion Question3;
    Question3.QuestionText = FText::FromString(TEXT("What does \"milk\" mean?"));
    Question3.AnswerA = FText::FromString(TEXT("Dziekuje"));
    Question3.AnswerB = FText::FromString(TEXT("Mleko"));
    Question3.AnswerC = FText::FromString(TEXT("Prosze"));
    Question3.CorrectAnswerIndex = 1;
    Questions.Add(Question3);

    CurrentQuestionIndex = 0;
    CorrectAnswersCount = 0;
    MistakesCount = 0;
    bCurrentQuestionHadMistake = false;
}

FQuizQuestion UQuizSubsystem::GetCurrentQuestion() const
{
    if (!Questions.IsValidIndex(CurrentQuestionIndex))
    {
        return FQuizQuestion();
    }

    return Questions[CurrentQuestionIndex];
}

EQuizAnswerResult UQuizSubsystem::SubmitAnswer(int32 SelectedAnswerIndex)
{
    if (!Questions.IsValidIndex(CurrentQuestionIndex))
    {
        return EQuizAnswerResult::Incorrect;
    }

    const FQuizQuestion& CurrentQuestion = Questions[CurrentQuestionIndex];

    if (SelectedAnswerIndex == CurrentQuestion.CorrectAnswerIndex)
    {
        if (!bCurrentQuestionHadMistake)
        {
            CorrectAnswersCount++;
        }

        return EQuizAnswerResult::Correct;
    }

    MistakesCount++;
    bCurrentQuestionHadMistake = true;

    return EQuizAnswerResult::Incorrect;
}

bool UQuizSubsystem::MoveToNextQuestion()
{
    CurrentQuestionIndex++;
    bCurrentQuestionHadMistake = false;

    return Questions.IsValidIndex(CurrentQuestionIndex);
}

bool UQuizSubsystem::IsQuizFinished() const
{
    return CurrentQuestionIndex >= Questions.Num();
}

int32 UQuizSubsystem::GetCorrectAnswersCount() const
{
    return CorrectAnswersCount;
}

int32 UQuizSubsystem::GetMistakesCount() const
{
    return MistakesCount;
}

int32 UQuizSubsystem::GetCurrentQuestionIndex() const
{
    return CurrentQuestionIndex;
}

int32 UQuizSubsystem::GetTotalQuestionsCount() const
{
    return Questions.Num();
}

bool UQuizSubsystem::IsQuizPassed() const
{
    return MistakesCount <= 1;
}