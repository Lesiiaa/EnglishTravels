// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ETDialogueTypes.h"
#include "DialogueSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class ENGLISHTRAVELS_API UDialogueSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
};