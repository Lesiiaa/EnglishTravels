#include "FlashcardSubsystem.h"
#include "Engine/GameInstance.h"
#include "VocabularySubsystem.h"

bool UFlashcardSubsystem::StartSession(FName SetId)
{
    UVocabularySubsystem* VocabularySubsystem =
        GetGameInstance()->GetSubsystem<UVocabularySubsystem>();

    return StartSessionFromWords(VocabularySubsystem
        ? VocabularySubsystem->GetWordsBySet(SetId) : TArray<FWordData>());
}

bool UFlashcardSubsystem::StartSessionByCategory(FName Category)
{
    UVocabularySubsystem* VocabularySubsystem =
        GetGameInstance()->GetSubsystem<UVocabularySubsystem>();

    return StartSessionFromWords(VocabularySubsystem
        ? VocabularySubsystem->GetWordsByCategory(Category) : TArray<FWordData>());
}

bool UFlashcardSubsystem::StartSessionFromWords(const TArray<FWordData>& Words)
{
    Cards.Empty();
    CurrentCardIndex = 0;
    RememberedCount = 0;
    ForgottenCount = 0;
    bCurrentCardRevealed = false;

    UVocabularySubsystem* VocabularySubsystem =
        GetGameInstance()->GetSubsystem<UVocabularySubsystem>();

    if (!VocabularySubsystem)
    {
        return false;
    }

    TSet<FName> AddedWordIds;

    for (const FWordData& Word : Words)
    {
        if (Word.WordId.IsNone() || Word.EnglishText.IsEmpty() ||
            Word.PolishText.IsEmpty() || AddedWordIds.Contains(Word.WordId) ||
            !VocabularySubsystem->GetWordProgress(Word.WordId).bDiscovered)
        {
            continue;
        }

        Cards.Add(Word);
        AddedWordIds.Add(Word.WordId);
    }

    return !Cards.IsEmpty();
}

FWordData UFlashcardSubsystem::GetCurrentCard() const
{
    return Cards.IsValidIndex(CurrentCardIndex) ? Cards[CurrentCardIndex] : FWordData();
}

bool UFlashcardSubsystem::RevealCurrentCard()
{
    if (!Cards.IsValidIndex(CurrentCardIndex))
    {
        return false;
    }

    bCurrentCardRevealed = true;
    return true;
}

bool UFlashcardSubsystem::SubmitAnswer(bool bRemembered)
{
    if (!Cards.IsValidIndex(CurrentCardIndex) || !bCurrentCardRevealed)
    {
        return false;
    }

    UVocabularySubsystem* VocabularySubsystem =
        GetGameInstance()->GetSubsystem<UVocabularySubsystem>();

    if (!VocabularySubsystem)
    {
        return false;
    }

    const FName WordId = Cards[CurrentCardIndex].WordId;

    if (bRemembered)
    {
        RememberedCount++;
        VocabularySubsystem->RegisterCorrectAnswer(WordId);
        VocabularySubsystem->AddKnowledgeScore(WordId, 1);
    }
    else
    {
        ForgottenCount++;
        VocabularySubsystem->RegisterMistake(WordId);
        VocabularySubsystem->AddKnowledgeScore(WordId, -1);
    }

    CurrentCardIndex++;
    bCurrentCardRevealed = false;
    return true;
}

bool UFlashcardSubsystem::IsCurrentCardRevealed() const
{
    return Cards.IsValidIndex(CurrentCardIndex) && bCurrentCardRevealed;
}

bool UFlashcardSubsystem::IsSessionFinished() const
{
    return !Cards.IsEmpty() && CurrentCardIndex >= Cards.Num();
}

int32 UFlashcardSubsystem::GetCurrentCardIndex() const
{
    return CurrentCardIndex;
}

int32 UFlashcardSubsystem::GetTotalCardsCount() const
{
    return Cards.Num();
}

int32 UFlashcardSubsystem::GetRememberedCount() const
{
    return RememberedCount;
}

int32 UFlashcardSubsystem::GetForgottenCount() const
{
    return ForgottenCount;
}
