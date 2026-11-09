#include "DialogueSubsystem.h"

void UDialogueSubsystem::EndDialogue()
{
    bDialogueActive = false;
    CurrentRowName = NAME_None;
}

void UDialogueSubsystem::SetDialogueTable(UDataTable* InDialogueTable)
{
    DialogueTable = InDialogueTable;
}

bool UDialogueSubsystem::StartDialogue(FName StartRowName)
{
    if (!DialogueTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("DialogueTable is not set."));
        return false;
    }

    const FDialogueLine* FoundLine = DialogueTable->FindRow<FDialogueLine>(StartRowName, TEXT("StartDialogue"));

    if (!FoundLine)
    {
        UE_LOG(LogTemp, Warning, TEXT("Dialogue row not found: %s"), *StartRowName.ToString());
        return false;
    }

    CurrentRowName = StartRowName;
    bDialogueActive = true;

    return true;
}

bool UDialogueSubsystem::NextLine()
{
    if (!DialogueTable || !bDialogueActive)
    {
        return false;
    }

    const FDialogueLine* CurrentLine = DialogueTable->FindRow<FDialogueLine>(CurrentRowName, TEXT("NextLine"));

    if (!CurrentLine)
    {
        bDialogueActive = false;
        return false;
    }

    if (CurrentLine->bEndsDialogue || CurrentLine->NextId.IsNone())
    {
        bDialogueActive = false;
        return false;
    }

    const FDialogueLine* NextDialogueLine = DialogueTable->FindRow<FDialogueLine>(CurrentLine->NextId, TEXT("NextLine"));

    if (!NextDialogueLine)
    {
        UE_LOG(LogTemp, Warning, TEXT("Next dialogue row not found: %s"), *CurrentLine->NextId.ToString());
        bDialogueActive = false;
        return false;
    }

    CurrentRowName = CurrentLine->NextId;

    return true;
}

FDialogueLine UDialogueSubsystem::GetCurrentLine() const
{
    if (!DialogueTable || !bDialogueActive)
    {
        return FDialogueLine();
    }

    const FDialogueLine* CurrentLine = DialogueTable->FindRow<FDialogueLine>(CurrentRowName, TEXT("GetCurrentLine"));

    if (!CurrentLine)
    {
        return FDialogueLine();
    }

    return *CurrentLine;
}

bool UDialogueSubsystem::IsDialogueActive() const
{
    return bDialogueActive;
}
