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

void UVocabularySubsystem::MarkWordAsDiscovered(FName WordId)
{
    if (WordId.IsNone())
    {
        return;
    }

    FWordProgress& Progress = WordProgress.FindOrAdd(WordId);
    Progress.bDiscovered = true;
}

void UVocabularySubsystem::RegisterCorrectAnswer(FName WordId)
{
    if (WordId.IsNone())
    {
        return;
    }

    FWordProgress& Progress = WordProgress.FindOrAdd(WordId);
    Progress.CorrectAnswers++;
}

void UVocabularySubsystem::RegisterMistake(FName WordId)
{
    if (WordId.IsNone())
    {
        return;
    }

    FWordProgress& Progress = WordProgress.FindOrAdd(WordId);
    Progress.Mistakes++;
}

FWordProgress UVocabularySubsystem::GetWordProgress(FName WordId) const
{
    if (const FWordProgress* Progress = WordProgress.Find(WordId))
    {
        return *Progress;
    }

    return FWordProgress();
}

void UVocabularySubsystem::AddKnowledgeScore(FName WordId, int32 Amount)
{
    if (WordId.IsNone())
    {
        return;
    }

    FWordProgress& Progress = WordProgress.FindOrAdd(WordId);

    Progress.KnowledgeScore = FMath::Clamp(
        Progress.KnowledgeScore + Amount,
        0,
        12
    );
}

EWordKnowledgeLevel UVocabularySubsystem::GetKnowledgeLevel(FName WordId) const
{
    const FWordProgress* Progress = WordProgress.Find(WordId);

    if (!Progress)
    {
        return EWordKnowledgeLevel::Unknown;
    }

    const int32 Score = Progress->KnowledgeScore;

    if (Score <= 0)
    {
        return EWordKnowledgeLevel::Unknown;
    }

    if (Score <= 3)
    {
        return EWordKnowledgeLevel::Weak;
    }

    if (Score <= 6)
    {
        return EWordKnowledgeLevel::Known;
    }

    if (Score <= 9)
    {
        return EWordKnowledgeLevel::WellKnown;
    }

    return EWordKnowledgeLevel::Mastered;
}

float UVocabularySubsystem::GetKnowledgePercent(FName WordId) const
{
    const FWordProgress* Progress = WordProgress.Find(WordId);

    if (!Progress)
    {
        return 0.0f;
    }

    return static_cast<float>(Progress->KnowledgeScore) / 12.0f;
}