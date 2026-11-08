#include "MemorySubsystem.h"
#include "Engine/GameInstance.h"
#include "VocabularySubsystem.h"

bool UMemorySubsystem::StartSession(int32 MaxPairs)
{
    UVocabularySubsystem* VocabularySubsystem =
        GetGameInstance()->GetSubsystem<UVocabularySubsystem>();

    return StartSessionFromWords(VocabularySubsystem
        ? VocabularySubsystem->GetDiscoveredWords() : TArray<FWordData>(), MaxPairs);
}

bool UMemorySubsystem::StartSessionByCategory(FName Category, int32 MaxPairs)
{
    UVocabularySubsystem* VocabularySubsystem =
        GetGameInstance()->GetSubsystem<UVocabularySubsystem>();

    return StartSessionFromWords(VocabularySubsystem
        ? (Category.IsNone() ? VocabularySubsystem->GetDiscoveredWords()
            : VocabularySubsystem->GetWordsByCategory(Category))
        : TArray<FWordData>(), MaxPairs);
}

bool UMemorySubsystem::StartSessionFromWords(const TArray<FWordData>& Words, int32 MaxPairs)
{
    EndSession();

    UVocabularySubsystem* VocabularySubsystem =
        GetGameInstance()->GetSubsystem<UVocabularySubsystem>();

    if (!VocabularySubsystem)
    {
        return false;
    }

    TArray<FWordData> EligibleWords;
    TSet<FName> AddedWordIds;

    for (const FWordData& Word : Words)
    {
        if (Word.WordId.IsNone() || Word.EnglishText.IsEmpty() || Word.Image.IsNull() ||
            AddedWordIds.Contains(Word.WordId) ||
            !VocabularySubsystem->GetWordProgress(Word.WordId).bDiscovered)
        {
            continue;
        }

        EligibleWords.Add(Word);
        AddedWordIds.Add(Word.WordId);
    }

    if (EligibleWords.Num() < 2)
    {
        return false;
    }

    for (int32 ShuffleIndex = EligibleWords.Num() - 1; ShuffleIndex > 0; ShuffleIndex--)
    {
        EligibleWords.Swap(ShuffleIndex, FMath::RandRange(0, ShuffleIndex));
    }

    const int32 PairCount = FMath::Min(EligibleWords.Num(), FMath::Clamp(MaxPairs, 2, 10));
    Cards.Reserve(PairCount * 2);

    for (int32 WordIndex = 0; WordIndex < PairCount; WordIndex++)
    {
        const FWordData& Word = EligibleWords[WordIndex];

        FMemoryCard WordCard;
        WordCard.WordId = Word.WordId;
        WordCard.CardType = EMemoryCardType::Word;
        WordCard.EnglishText = Word.EnglishText;
        Cards.Add(WordCard);

        FMemoryCard ImageCard;
        ImageCard.WordId = Word.WordId;
        ImageCard.CardType = EMemoryCardType::Image;
        ImageCard.Image = Word.Image;
        Cards.Add(ImageCard);
    }

    for (int32 ShuffleIndex = Cards.Num() - 1; ShuffleIndex > 0; ShuffleIndex--)
    {
        Cards.Swap(ShuffleIndex, FMath::RandRange(0, ShuffleIndex));
    }

    return true;
}

EMemorySelectionResult UMemorySubsystem::SelectCard(int32 CardIndex)
{
    if (bBoardLocked || !Cards.IsValidIndex(CardIndex) ||
        Cards[CardIndex].bRevealed || Cards[CardIndex].bMatched)
    {
        return EMemorySelectionResult::Ignored;
    }

    Cards[CardIndex].bRevealed = true;

    if (FirstCardIndex == INDEX_NONE)
    {
        FirstCardIndex = CardIndex;
        return EMemorySelectionResult::FirstCardRevealed;
    }

    SecondCardIndex = CardIndex;
    FMemoryCard& FirstCard = Cards[FirstCardIndex];
    FMemoryCard& SecondCard = Cards[SecondCardIndex];

    if (FirstCard.WordId == SecondCard.WordId && FirstCard.CardType != SecondCard.CardType)
    {
        FirstCard.bMatched = true;
        SecondCard.bMatched = true;
        MatchedPairsCount++;
        FirstCardIndex = INDEX_NONE;
        SecondCardIndex = INDEX_NONE;
        return EMemorySelectionResult::Match;
    }

    bBoardLocked = true;
    return EMemorySelectionResult::Mismatch;
}

bool UMemorySubsystem::ResolveMismatch()
{
    if (!bBoardLocked || !Cards.IsValidIndex(FirstCardIndex) ||
        !Cards.IsValidIndex(SecondCardIndex))
    {
        return false;
    }

    Cards[FirstCardIndex].bRevealed = false;
    Cards[SecondCardIndex].bRevealed = false;
    FirstCardIndex = INDEX_NONE;
    SecondCardIndex = INDEX_NONE;
    bBoardLocked = false;
    return true;
}

void UMemorySubsystem::EndSession()
{
    Cards.Empty();
    FirstCardIndex = INDEX_NONE;
    SecondCardIndex = INDEX_NONE;
    MatchedPairsCount = 0;
    bBoardLocked = false;
}

TArray<FMemoryCard> UMemorySubsystem::GetCards() const
{
    return Cards;
}

FMemoryCard UMemorySubsystem::GetCard(int32 CardIndex) const
{
    return Cards.IsValidIndex(CardIndex) ? Cards[CardIndex] : FMemoryCard();
}

bool UMemorySubsystem::IsBoardLocked() const
{
    return bBoardLocked;
}

bool UMemorySubsystem::IsSessionFinished() const
{
    return !Cards.IsEmpty() && MatchedPairsCount == GetTotalPairsCount();
}

int32 UMemorySubsystem::GetMatchedPairsCount() const
{
    return MatchedPairsCount;
}

int32 UMemorySubsystem::GetTotalPairsCount() const
{
    return Cards.Num() / 2;
}

int32 UMemorySubsystem::GetBoardColumns() const
{
    const int32 PairCount = GetTotalPairsCount();

    if (PairCount <= 2)
    {
        return 2;
    }

    if (PairCount == 3)
    {
        return 3;
    }

    return PairCount <= 6 ? 4 : 5;
}

FVector2D UMemorySubsystem::GetCardSize() const
{
    switch (GetBoardColumns())
    {
    case 2:
        return FVector2D(180.0f, 180.0f);
    case 3:
        return FVector2D(160.0f, 160.0f);
    case 4:
        return FVector2D(140.0f, 140.0f);
    default:
        return FVector2D(110.0f, 100.0f);
    }
}
