#include "QuizSubsystem.h"
#include "Engine/DataTable.h"
#include "VocabularySubsystem.h"

void UQuizSubsystem::StartQuiz()
{
    Questions.Empty();

    if (!WordTable)
    {
        CurrentQuestionIndex = 0;
        CorrectAnswersCount = 0;
        MistakesCount = 0;
        bCurrentQuestionHadMistake = false;
        bQuizCompleted = false;
        return;
    }

    TArray<FWordData> ShopWords;

    const TArray<FName> RowNames = WordTable->GetRowNames();

    for (const FName& RowName : RowNames)
    {
        const FWordData* Word = WordTable->FindRow<FWordData>(
            RowName,
            TEXT("StartQuiz")
        );

        if (Word && Word->SetId == FName("Shop"))
        {
            ShopWords.Add(*Word);
        }
    }

    for (int32 i = 0; i < ShopWords.Num(); i++)
    {
        const FWordData& CurrentWord = ShopWords[i];

        FQuizQuestion Question;

        Question.WordId = CurrentWord.WordId;

        Question.QuestionText = FText::Format(
            FText::FromString(TEXT("What does \"{0}\" mean?")),
            CurrentWord.EnglishText
        );

        TArray<FText> Answers;

        Answers.Add(CurrentWord.PolishText);
        Answers.Add(ShopWords[(i + 1) % ShopWords.Num()].PolishText);
        Answers.Add(ShopWords[(i + 2) % ShopWords.Num()].PolishText);

        const FText CorrectAnswer = Answers[0];

        for (int32 j = Answers.Num() - 1; j > 0; j--)
        {
            const int32 RandomIndex = FMath::RandRange(0, j);
            Answers.Swap(j, RandomIndex);
        }

        Question.AnswerA = Answers[0];
        Question.AnswerB = Answers[1];
        Question.AnswerC = Answers[2];

        if (Answers[0].EqualTo(CorrectAnswer))
        {
            Question.CorrectAnswerIndex = 0;
        }
        else if (Answers[1].EqualTo(CorrectAnswer))
        {
            Question.CorrectAnswerIndex = 1;
        }
        else
        {
            Question.CorrectAnswerIndex = 2;
        }

        Questions.Add(Question);
    }

    CurrentQuestionIndex = 0;
    CorrectAnswersCount = 0;
    MistakesCount = 0;
    bCurrentQuestionHadMistake = false;
    bQuizCompleted = false;
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

    UVocabularySubsystem* VocabularySubsystem =
        GetGameInstance()->GetSubsystem<UVocabularySubsystem>();

    if (SelectedAnswerIndex == CurrentQuestion.CorrectAnswerIndex)
    {
        if (!bCurrentQuestionHadMistake)
        {
            CorrectAnswersCount++;

            if (VocabularySubsystem)
            {
                VocabularySubsystem->RegisterCorrectAnswer(CurrentQuestion.WordId);
            }
        }

        return EQuizAnswerResult::Correct;
    }

    MistakesCount++;
    bCurrentQuestionHadMistake = true;

    if (VocabularySubsystem)
    {
        VocabularySubsystem->RegisterMistake(CurrentQuestion.WordId);
    }

    return EQuizAnswerResult::Incorrect;
}

bool UQuizSubsystem::MoveToNextQuestion()
{
    CurrentQuestionIndex++;
    bCurrentQuestionHadMistake = false;

    if (!Questions.IsValidIndex(CurrentQuestionIndex))
    {
        bQuizCompleted = true;
        return false;
    }

    return true;
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

bool UQuizSubsystem::HasCompletedQuiz() const
{
    return bQuizCompleted;
}

bool UQuizSubsystem::HasPassedQuiz() const
{
    return bQuizCompleted && IsQuizPassed();
}

void UQuizSubsystem::SetWordTable(UDataTable* NewWordTable)
{
    WordTable = NewWordTable;
}