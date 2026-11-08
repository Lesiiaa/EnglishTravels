#pragma once

#include "CoreMinimal.h"
#include "MemoryTypes.generated.h"

class UTexture2D;

UENUM(BlueprintType)
enum class EMemoryCardType : uint8
{
    Word,
    Image
};

UENUM(BlueprintType)
enum class EMemorySelectionResult : uint8
{
    Ignored,
    FirstCardRevealed,
    Match,
    Mismatch
};

USTRUCT(BlueprintType)
struct ENGLISHTRAVELS_API FMemoryCard
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Memory")
    FName WordId;

    UPROPERTY(BlueprintReadOnly, Category = "Memory")
    EMemoryCardType CardType = EMemoryCardType::Word;

    UPROPERTY(BlueprintReadOnly, Category = "Memory")
    FText EnglishText;

    UPROPERTY(BlueprintReadOnly, Category = "Memory")
    TSoftObjectPtr<UTexture2D> Image;

    UPROPERTY(BlueprintReadOnly, Category = "Memory")
    bool bRevealed = false;

    UPROPERTY(BlueprintReadOnly, Category = "Memory")
    bool bMatched = false;
};
