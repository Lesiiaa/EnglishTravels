#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "WordTypes.generated.h"

class UTexture2D;

UENUM(BlueprintType)
enum class EStudyActivity : uint8
{
    Flashcards UMETA(DisplayName = "Flashcards"),
    Quiz UMETA(DisplayName = "Quiz"),
    Memory UMETA(DisplayName = "Memory")
};

USTRUCT(BlueprintType)
struct ENGLISHTRAVELS_API FWordData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Word")
    FName WordId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Word")
    FText EnglishText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Word")
    FText PolishText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Word")
    FName SetId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Word")
    TArray<FName> Categories;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Word")
    TSoftObjectPtr<UTexture2D> Image;
};

UENUM(BlueprintType)
enum class EWordKnowledgeLevel : uint8
{
    Unknown UMETA(DisplayName = "Nieznane"),
    Weak UMETA(DisplayName = "Slabo znane"),
    Known UMETA(DisplayName = "Znane"),
    WellKnown UMETA(DisplayName = "Dobrze znane"),
    Mastered UMETA(DisplayName = "Mastered")
};

USTRUCT(BlueprintType)
struct ENGLISHTRAVELS_API FWordProgress
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Progress")
    bool bDiscovered = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Progress")
    int32 KnowledgeScore = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Progress")
    int32 CorrectAnswers = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Progress")
    int32 Mistakes = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Review")
    FDateTime FirstDiscoveredUtc;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Review")
    FDateTime LastReviewUtc;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Review")
    FDateTime NextReviewUtc;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Review")
    int32 ReviewStreak = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Review")
    int32 ReviewCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Review")
    int32 ForgottenReviews = 0;
};
