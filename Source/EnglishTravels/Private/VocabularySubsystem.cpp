#include "VocabularySubsystem.h"
#include "Engine/DataTable.h"

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

void UVocabularySubsystem::SetWordTable(UDataTable* NewWordTable)
{
    WordTable = NewWordTable;
}

FWordData UVocabularySubsystem::GetWordById(FName WordId) const
{
    if (!WordTable || WordId.IsNone())
    {
        return FWordData();
    }

    const FWordData* Word = WordTable->FindRow<FWordData>(
        WordId,
        TEXT("GetWordById")
    );

    if (!Word)
    {
        return FWordData();
    }

    return *Word;
}

TArray<FWordData> UVocabularySubsystem::GetWordsBySet(FName SetId) const
{
    TArray<FWordData> Result;

    if (!WordTable || SetId.IsNone())
    {
        return Result;
    }

    const TArray<FName> RowNames = WordTable->GetRowNames();

    for (const FName& RowName : RowNames)
    {
        const FWordData* Word = WordTable->FindRow<FWordData>(
            RowName,
            TEXT("GetWordsBySet")
        );

        if (Word && Word->SetId == SetId)
        {
            Result.Add(*Word);
        }
    }

    return Result;
}

TArray<FWordData> UVocabularySubsystem::GetWordsByCategory(FName Category) const
{
    TArray<FWordData> Result;

    if (!WordTable || Category.IsNone())
    {
        return Result;
    }

    const TArray<FName> RowNames = WordTable->GetRowNames();

    for (const FName& RowName : RowNames)
    {
        const FWordData* Word = WordTable->FindRow<FWordData>(
            RowName,
            TEXT("GetWordsByCategory")
        );

        if (Word && Word->Categories.Contains(Category))
        {
            Result.Add(*Word);
        }
    }

    return Result;
}