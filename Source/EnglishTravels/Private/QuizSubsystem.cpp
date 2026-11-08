#include "QuizSubsystem.h"
#include "Engine/DataTable.h"
#include "VocabularySubsystem.h"

void UQuizSubsystem::StartQuiz()
{
    bQuizCompleted = false;
    bStoryQuizPassed = false;
    BuildQuiz(FName("Shop"), false);
}

bool UQuizSubsystem::StartTrainingQuiz(FName SetId)
{
    return BuildQuiz(SetId, true);
}

bool UQuizSubsystem::StartTrainingQuizAllWords()
{
    return BuildQuiz(NAME_None, true);
}

bool UQuizSubsystem::StartTrainingQuizByCategory(FName Category)
{
    return BuildQuiz(NAME_None, true, Category);
}

bool UQuizSubsystem::BuildQuiz(FName SetId, bool bOnlyDiscovered, FName Category)
{
    Questions.Empty();
    CurrentQuestionIndex = 0;
    CorrectAnswersCount = 0;
    MistakesCount = 0;
    bCurrentQuestionHadMistake = false;
    bTrainingMode = bOnlyDiscovered;

    if (!WordTable || (SetId.IsNone() && !bOnlyDiscovered))
    {
        return false;
    }

    TArray<FWordData> ShopWords;
    UVocabularySubsystem* VocabularySubsystem =
        GetGameInstance()->GetSubsystem<UVocabularySubsystem>();

    if (bOnlyDiscovered && !VocabularySubsystem)
    {
        return false;
    }

    const TArray<FName> RowNames = WordTable->GetRowNames();

    for (const FName& RowName : RowNames)
    {
        const FWordData* Word = WordTable->FindRow<FWordData>(
            RowName,
            TEXT("StartQuiz")
        );

        if (Word && (Word->SetId == SetId || (bOnlyDiscovered && SetId.IsNone())))
        {
            if (!Category.IsNone() && !Word->Categories.Contains(Category))
            {
                continue;
            }

            if (bOnlyDiscovered)
            {
                if (Word->WordId.IsNone() || Word->EnglishText.IsEmpty() ||
                    Word->PolishText.IsEmpty() ||
                    !VocabularySubsystem->GetWordProgress(Word->WordId).bDiscovered)
                {
                    continue;
                }

                const bool bDuplicateAnswer = ShopWords.ContainsByPredicate(
                    [Word](const FWordData& ExistingWord)
                    {
                        return ExistingWord.WordId == Word->WordId ||
                            ExistingWord.PolishText.EqualTo(Word->PolishText);
                    });

                if (bDuplicateAnswer)
                {
                    continue;
                }
            }

            ShopWords.Add(*Word);
        }
    }

    if (bOnlyDiscovered && ShopWords.Num() < 3)
    {
        return false;
    }

    if (bOnlyDiscovered)
    {
        for (int32 ShuffleIndex = ShopWords.Num() - 1; ShuffleIndex > 0; ShuffleIndex--)
        {
            ShopWords.Swap(ShuffleIndex, FMath::RandRange(0, ShuffleIndex));
        }
    }

    const int32 QuestionCount = bOnlyDiscovered ? FMath::Min(ShopWords.Num(), 10) : ShopWords.Num();

    for (int32 WordIndex = 0; WordIndex < QuestionCount; WordIndex++)
    {
        const FWordData& CurrentWord = ShopWords[WordIndex];

        FQuizQuestion Question;

        Question.WordId = CurrentWord.WordId;

        Question.QuestionText = FText::Format(
            FText::FromString(TEXT("What does \"{0}\" mean?")),
            CurrentWord.EnglishText
        );

        TArray<FText> Answers;

        Answers.Add(CurrentWord.PolishText);
        Answers.Add(ShopWords[(WordIndex + 1) % ShopWords.Num()].PolishText);
        Answers.Add(ShopWords[(WordIndex + 2) % ShopWords.Num()].PolishText);

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

    return !Questions.IsEmpty();
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
        if (VocabularySubsystem)
        {
            VocabularySubsystem->RegisterCorrectAnswer(CurrentQuestion.WordId);

            if (!bCurrentQuestionHadMistake)
            {
                VocabularySubsystem->AddKnowledgeScore(CurrentQuestion.WordId, 1);
            }
        }

        if (!bCurrentQuestionHadMistake)
        {
            CorrectAnswersCount++;
        }

        return EQuizAnswerResult::Correct;
    }

    MistakesCount++;
    bCurrentQuestionHadMistake = true;

    if (VocabularySubsystem)
    {
        VocabularySubsystem->RegisterMistake(CurrentQuestion.WordId);
        VocabularySubsystem->AddKnowledgeScore(CurrentQuestion.WordId, -1);
    }

    return EQuizAnswerResult::Incorrect;
}

bool UQuizSubsystem::MoveToNextQuestion()
{
    if (!Questions.IsValidIndex(CurrentQuestionIndex))
    {
        return false;
    }

    CurrentQuestionIndex++;
    bCurrentQuestionHadMistake = false;

    if (!Questions.IsValidIndex(CurrentQuestionIndex))
    {
        if (!bTrainingMode)
        {
            bQuizCompleted = true;
            bStoryQuizPassed = IsQuizPassed();
        }
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
    return bQuizCompleted && bStoryQuizPassed;
}

void UQuizSubsystem::SetWordTable(UDataTable* NewWordTable)
{
    WordTable = NewWordTable;
}
