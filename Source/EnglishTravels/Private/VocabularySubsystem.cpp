#include "VocabularySubsystem.h"

bool UVocabularySubsystem::MarkWordAsLearned(FName WordId)
{
    if (WordId.IsNone())
    {
        return false;
    }

    if (LearnedWords.Contains(WordId))
    {
        return false;
    }

    LearnedWords.Add(WordId);
    return true;
}

bool UVocabularySubsystem::IsWordLearned(FName WordId) const
{
    return LearnedWords.Contains(WordId);
}

int32 UVocabularySubsystem::GetLearnedWordsCount() const
{
    return LearnedWords.Num();
}

int32 UVocabularySubsystem::GetTotalWordsCount() const
{
    return TotalWordsCount;
}

bool UVocabularySubsystem::AreAllWordsLearned() const
{
    return TotalWordsCount > 0 && LearnedWords.Num() >= TotalWordsCount;
}

void UVocabularySubsystem::SetTotalWordsCount(int32 NewTotalWordsCount)
{
    TotalWordsCount = FMath::Max(0, NewTotalWordsCount);
}