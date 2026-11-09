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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FText SpeakerName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FText OriginalText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FText Translation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    bool bShowTranslationButton = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TArray<FDialogueChoice> Choices;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FName NextId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    bool bEndsDialogue = false;
};
