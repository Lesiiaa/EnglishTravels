#include "WritingSubsystem.h"
#include "Engine/GameInstance.h"
#include "VocabularySubsystem.h"

bool UWritingSubsystem::StartSessionByCategory(FName Category, int32 MaxWords)
{
    EndSession();

    UVocabularySubsystem* VocabularySubsystem =
        GetGameInstance()->GetSubsystem<UVocabularySubsystem>();

    if (!VocabularySubsystem)
    {
        return false;
    }

    const TArray<FWordData> SourceWords = Category.IsNone()
        ? VocabularySubsystem->GetDiscoveredWords()
        : VocabularySubsystem->GetWordsByCategory(Category);
    TSet<FName> AddedWordIds;

    //select up to five discovered words from the chosen material
    for (const FWordData& Word : SourceWords)
    {
        if (Word.WordId.IsNone() || Word.EnglishText.ToString().TrimStartAndEnd().IsEmpty() ||
            Word.PolishText.ToString().TrimStartAndEnd().IsEmpty() ||
            AddedWordIds.Contains(Word.WordId) ||
            !VocabularySubsystem->GetWordProgress(Word.WordId).bDiscovered)
        {
            continue;
        }

        Words.Add(Word);
        AddedWordIds.Add(Word.WordId);
    }

    for (int32 ShuffleIndex = Words.Num() - 1; ShuffleIndex > 0; ShuffleIndex--)
    {
        Words.Swap(ShuffleIndex, FMath::RandRange(0, ShuffleIndex));
    }

    const int32 WordLimit = FMath::Clamp(MaxWords, 1, 5);
    if (Words.Num() > WordLimit)
    {
        Words.SetNum(WordLimit);
    }

    return !Words.IsEmpty();
}

EWritingAnswerResult UWritingSubsystem::SubmitAnswer(const FString& Answer)
{
    if (!Words.IsValidIndex(CurrentWordIndex) || bCurrentWordAnswered)
    {
        return EWritingAnswerResult::Ignored;
    }

    const FString SubmittedAnswer = Answer.TrimStartAndEnd();
    if (SubmittedAnswer.IsEmpty())
    {
        return EWritingAnswerResult::EmptyAnswer;
    }

    //ignore letter case and surrounding whitespace, but require the stored spelling
    const FString ExpectedAnswer = Words[CurrentWordIndex].EnglishText.ToString().TrimStartAndEnd();
    const bool bCorrect = SubmittedAnswer.Equals(ExpectedAnswer, ESearchCase::IgnoreCase);
    UVocabularySubsystem* VocabularySubsystem =
        GetGameInstance()->GetSubsystem<UVocabularySubsystem>();

    if (!VocabularySubsystem || !VocabularySubsystem->RecordPracticeAnswer(
        Words[CurrentWordIndex].WordId, bCorrect, bCorrect ? 1 : -1))
    {
        return EWritingAnswerResult::Ignored;
    }

    bCurrentWordAnswered = true;

    //record session results after updating and saving vocabulary progress
    if (bCorrect)
    {
        CorrectAnswersCount++;
        return EWritingAnswerResult::Correct;
    }

    IncorrectAnswersCount++;
    return EWritingAnswerResult::Incorrect;
}

bool UWritingSubsystem::SkipCurrentWord()
{
    if (!Words.IsValidIndex(CurrentWordIndex) || bCurrentWordAnswered)
    {
        return false;
    }

    UVocabularySubsystem* VocabularySubsystem =
        GetGameInstance()->GetSubsystem<UVocabularySubsystem>();

    if (!VocabularySubsystem || !VocabularySubsystem->RecordPracticeAnswer(
        Words[CurrentWordIndex].WordId, false, -1))
    {
        return false;
    }

    SkippedWordsCount++;
    bCurrentWordAnswered = true;
    return true;
}

bool UWritingSubsystem::MoveToNextWord()
{
    if (!Words.IsValidIndex(CurrentWordIndex) || !bCurrentWordAnswered)
    {
        return false;
    }

    //leave the answer visible until the player chooses to continue
    CurrentWordIndex++;
    bCurrentWordAnswered = false;
    return Words.IsValidIndex(CurrentWordIndex);
}

void UWritingSubsystem::EndSession()
{
    Words.Empty();
    CurrentWordIndex = 0;
    CorrectAnswersCount = 0;
    IncorrectAnswersCount = 0;
    SkippedWordsCount = 0;
    bCurrentWordAnswered = false;
}

FWordData UWritingSubsystem::GetCurrentWord() const
{
    return Words.IsValidIndex(CurrentWordIndex) ? Words[CurrentWordIndex] : FWordData();
}

bool UWritingSubsystem::IsCurrentWordAnswered() const
{
    return Words.IsValidIndex(CurrentWordIndex) && bCurrentWordAnswered;
}

bool UWritingSubsystem::IsSessionFinished() const
{
    return !Words.IsEmpty() && CurrentWordIndex >= Words.Num();
}

int32 UWritingSubsystem::GetCurrentWordIndex() const
{
    return CurrentWordIndex;
}

int32 UWritingSubsystem::GetTotalWordsCount() const
{
    return Words.Num();
}

int32 UWritingSubsystem::GetCorrectAnswersCount() const
{
    return CorrectAnswersCount;
}

int32 UWritingSubsystem::GetIncorrectAnswersCount() const
{
    return IncorrectAnswersCount;
}

int32 UWritingSubsystem::GetSkippedWordsCount() const
{
    return SkippedWordsCount;
}
