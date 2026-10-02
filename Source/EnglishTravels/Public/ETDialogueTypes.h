#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ETDialogueTypes.generated.h"

USTRUCT(BlueprintType)
struct ENGLISHTRAVELS_API FDialogueChoice
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FText ChoiceText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    bool bIsCorrect = false;
};

USTRUCT(BlueprintType)
struct ENGLISHTRAVELS_API FDialogueLine : public FTableRowBase
{
    GENERATED_BODY()

    /** Unikalny identyfikator dialogu */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FName Id;

    /** Nazwa mówi¹cej postaci */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FText SpeakerName;

    /** Oryginalny tekst */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FText OriginalText;

    /** T³umaczenie ca³ego zdania */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FText Translation;

    /** Czy pokazaæ przycisk "Translation" */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    bool bShowTranslationButton = true;

    /** Odpowiedzi (puste dla zwyk³ych dialogów) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TArray<FDialogueChoice> Choices;

    /** Id nastêpnej linijki dialogu */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FName NextId;

    /** Czy ta linia koñczy dialog */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    bool bEndsDialogue = false;
};