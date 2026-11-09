#include "VocabularySubsystem.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "GameProgressSubsystem.h"

bool UVocabularySubsystem::MarkWordAsLearned(FName WordId)
{
    if (WordId.IsNone())
    {
        return false;
    }

    if (IsWordLearned(WordId))
    {
        return false;
    }

    MarkWordAsDiscovered(WordId);
    return true;
}

bool UVocabularySubsystem::IsWordLearned(FName WordId) const
{
    const FWordProgress* Progress = WordProgress.Find(WordId);
    return Progress && Progress->bDiscovered;
}

int32 UVocabularySubsystem::GetLearnedWordsCount() const
{
    int32 Count = 0;
    for (const TPair<FName, FWordProgress>& Entry : WordProgress)
    {
        if (Entry.Value.bDiscovered)
        {
            Count++;
        }
    }
    return Count;
}

int32 UVocabularySubsystem::GetTotalWordsCount() const
{
    return TotalWordsCount;
}

bool UVocabularySubsystem::AreAllWordsLearned() const
{
    return TotalWordsCount > 0 && GetLearnedWordsCount() >= TotalWordsCount;
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

TArray<FName> UVocabularySubsystem::GetDiscoveredCategories() const
{
    TArray<FName> Result;

    if (!WordTable)
    {
        return Result;
    }

    TSet<FName> UniqueCategories;

    for (const FName& RowName : WordTable->GetRowNames())
    {
        const FWordData* Word = WordTable->FindRow<FWordData>(
            RowName, TEXT("GetDiscoveredCategories"));

        if (!Word || Word->WordId.IsNone() || Word->EnglishText.IsEmpty() ||
            Word->PolishText.IsEmpty() || !GetWordProgress(Word->WordId).bDiscovered)
        {
            continue;
        }

        for (const FName& Category : Word->Categories)
        {
            if (!Category.IsNone())
            {
                UniqueCategories.Add(Category);
            }
        }
    }

    Result = UniqueCategories.Array();
    Result.Sort([](const FName& FirstCategory, const FName& SecondCategory)
    {
        return FirstCategory.LexicalLess(SecondCategory);
    });

    return Result;
}

TArray<FWordData> UVocabularySubsystem::GetDiscoveredWords() const
{
    TArray<FWordData> Result;

    if (!WordTable)
    {
        return Result;
    }

    for (const FName& RowName : WordTable->GetRowNames())
    {
        const FWordData* Word = WordTable->FindRow<FWordData>(
            RowName, TEXT("GetDiscoveredWords"));

        if (Word && !Word->WordId.IsNone() && GetWordProgress(Word->WordId).bDiscovered)
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
    if (Progress.bDiscovered)
    {
        return;
    }

    //make newly discovered words available for review immediately
    Progress.bDiscovered = true;
    Progress.FirstDiscoveredUtc = FDateTime::UtcNow();
    Progress.NextReviewUtc = Progress.FirstDiscoveredUtc;
    NotifyProgressChanged();
}

void UVocabularySubsystem::RegisterCorrectAnswer(FName WordId)
{
    if (WordId.IsNone())
    {
        return;
    }

    FWordProgress& Progress = WordProgress.FindOrAdd(WordId);
    Progress.CorrectAnswers++;
    NotifyProgressChanged();
}

void UVocabularySubsystem::RegisterMistake(FName WordId)
{
    if (WordId.IsNone())
    {
        return;
    }

    FWordProgress& Progress = WordProgress.FindOrAdd(WordId);
    Progress.Mistakes++;
    NotifyProgressChanged();
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
    NotifyProgressChanged();
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

bool UVocabularySubsystem::RecordPracticeAnswer(FName WordId, bool bCorrect, int32 ScoreChange)
{
    if (WordId.IsNone())
    {
        return false;
    }

    FWordProgress& Progress = WordProgress.FindOrAdd(WordId);
    if (!Progress.bDiscovered)
    {
        Progress.bDiscovered = true;
        Progress.FirstDiscoveredUtc = FDateTime::UtcNow();
        Progress.NextReviewUtc = Progress.FirstDiscoveredUtc;
    }

    if (bCorrect)
    {
        Progress.CorrectAnswers++;
    }
    else
    {
        Progress.Mistakes++;
    }

    Progress.KnowledgeScore = FMath::Clamp(Progress.KnowledgeScore + ScoreChange, 0, 12);
    NotifyProgressChanged();
    return true;
}

TArray<FWordData> UVocabularySubsystem::GetDueReviewWords(int32 MaxWords) const
{
    TArray<FWordData> Result;
    const FDateTime NowUtc = FDateTime::UtcNow();
    TSet<FName> AddedWordIds;

    for (const FWordData& Word : GetDiscoveredWords())
    {
        const FWordProgress& Progress = WordProgress.FindChecked(Word.WordId);
        if (!Word.EnglishText.IsEmpty() && !Word.PolishText.IsEmpty() &&
            Progress.NextReviewUtc <= NowUtc && !AddedWordIds.Contains(Word.WordId))
        {
            Result.Add(Word);
            AddedWordIds.Add(Word.WordId);
        }
    }

    //prioritize weaker words, then earlier review dates
    Result.Sort([this](const FWordData& First, const FWordData& Second)
    {
        const FWordProgress& FirstProgress = WordProgress.FindChecked(First.WordId);
        const FWordProgress& SecondProgress = WordProgress.FindChecked(Second.WordId);
        if (FirstProgress.KnowledgeScore != SecondProgress.KnowledgeScore)
        {
            return FirstProgress.KnowledgeScore < SecondProgress.KnowledgeScore;
        }

        if (FirstProgress.NextReviewUtc != SecondProgress.NextReviewUtc)
        {
            return FirstProgress.NextReviewUtc < SecondProgress.NextReviewUtc;
        }

        return First.WordId.LexicalLess(Second.WordId);
    });

    if (MaxWords > 0 && Result.Num() > MaxWords)
    {
        Result.SetNum(MaxWords);
    }

    return Result;
}

int32 UVocabularySubsystem::GetDueReviewCount() const
{
    return GetDueReviewWords().Num();
}

bool UVocabularySubsystem::RegisterReviewResult(FName WordId, bool bRemembered)
{
    FWordProgress* Progress = WordProgress.Find(WordId);
    const FDateTime NowUtc = FDateTime::UtcNow();
    if (!Progress || !Progress->bDiscovered || Progress->NextReviewUtc > NowUtc)
    {
        return false;
    }

    //schedule reviews using real utc dates, including time outside the game
    Progress->LastReviewUtc = NowUtc;
    Progress->ReviewCount++;
    if (bRemembered)
    {
        Progress->CorrectAnswers++;
        Progress->KnowledgeScore = FMath::Min(12, Progress->KnowledgeScore + 1);
        Progress->ReviewStreak++;
        const int32 Score = Progress->KnowledgeScore;
        const int32 IntervalDays = Score <= 3 ? 1 : Score <= 6 ? 3 :
            Score <= 9 ? 7 : Score < 12 ? 14 : 30;
        Progress->NextReviewUtc = NowUtc + FTimespan::FromDays(IntervalDays);
    }
    else
    {
        Progress->Mistakes++;
        Progress->ForgottenReviews++;
        Progress->KnowledgeScore = FMath::Max(0, Progress->KnowledgeScore - 1);
        Progress->ReviewStreak = 0;
        Progress->NextReviewUtc = NowUtc + FTimespan::FromMinutes(10);
    }

    NotifyProgressChanged();
    return true;
}

int32 UVocabularySubsystem::GetDiscoveredWordsCountInSet(FName SetId) const
{
    TSet<FName> DiscoveredIds;
    for (const FWordData& Word : GetWordsBySet(SetId))
    {
        if (!Word.WordId.IsNone() && GetWordProgress(Word.WordId).bDiscovered)
        {
            DiscoveredIds.Add(Word.WordId);
        }
    }

    return DiscoveredIds.Num();
}

bool UVocabularySubsystem::AreAllWordsInSetDiscovered(FName SetId) const
{
    const TArray<FWordData> Words = GetWordsBySet(SetId);
    if (Words.IsEmpty())
    {
        return false;
    }

    for (const FWordData& Word : Words)
    {
        if (Word.WordId.IsNone() || !GetWordProgress(Word.WordId).bDiscovered)
        {
            return false;
        }
    }

    return true;
}

TMap<FName, FWordProgress> UVocabularySubsystem::ExportProgress() const
{
    return WordProgress;
}

void UVocabularySubsystem::RestoreProgress(const TMap<FName, FWordProgress>& SavedProgress)
{
    WordProgress = SavedProgress;
    WordProgress.Remove(NAME_None);
    for (TPair<FName, FWordProgress>& Entry : WordProgress)
    {
        FWordProgress& Progress = Entry.Value;
        Progress.KnowledgeScore = FMath::Clamp(Progress.KnowledgeScore, 0, 12);
        Progress.CorrectAnswers = FMath::Max(0, Progress.CorrectAnswers);
        Progress.Mistakes = FMath::Max(0, Progress.Mistakes);
        Progress.ReviewCount = FMath::Max(0, Progress.ReviewCount);
        Progress.ReviewStreak = FMath::Max(0, Progress.ReviewStreak);
        Progress.ForgottenReviews = FMath::Max(0, Progress.ForgottenReviews);
    }
}

void UVocabularySubsystem::NotifyProgressChanged()
{
    if (UGameProgressSubsystem* ProgressSubsystem = GetGameInstance()->GetSubsystem<UGameProgressSubsystem>())
    {
        ProgressSubsystem->AutoSave();
    }
}
