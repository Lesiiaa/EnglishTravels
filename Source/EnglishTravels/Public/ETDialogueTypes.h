#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ETDialogueTypes.generated.h"

USTRUCT(BlueprintType)
struct ENGLISHTRAVELS_API FDialogueChoice
{
    GENERATED_BODY()

    /** Answer text */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FText ChoiceText;

    /** Is it the correct answer */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    bool bIsCorrect = false;
};

USTRUCT(BlueprintType)
struct ENGLISHTRAVELS_API FDialogueLine : public FTableRowBase
{
    GENERATED_BODY()

    /** NPC name */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FText SpeakerName;

    /** English text */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FText OriginalText;

    /** Polish Translation */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FText Translation;

    /** Is Translation Avaiable? */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    bool bShowTranslationButton = true;

    /** Answer choices, empty when no choices*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TArray<FDialogueChoice> Choices;

    /** Next dialogue line, Row Name from DataTable */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FName NextId;

    /** This line ends dialogue? */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    bool bEndsDialogue = false;
};