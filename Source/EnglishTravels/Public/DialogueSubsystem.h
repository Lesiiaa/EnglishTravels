#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "ETDialogueTypes.h"
#include "DialogueSubsystem.generated.h"

UCLASS()
class ENGLISHTRAVELS_API UDialogueSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    void SetDialogueTable(UDataTable* InDialogueTable);

    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    bool StartDialogue(FName StartRowName);

    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    bool NextLine();

    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    FDialogueLine GetCurrentLine() const;

    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    bool IsDialogueActive() const;

    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    void EndDialogue();

private:
    UPROPERTY()
    UDataTable* DialogueTable = nullptr;

    UPROPERTY()
    FName CurrentRowName;

    UPROPERTY()
    bool bDialogueActive = false;
};
