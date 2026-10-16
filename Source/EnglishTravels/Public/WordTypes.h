#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "WordTypes.generated.h"

class UTexture2D;

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

USTRUCT(BlueprintType)
struct ENGLISHTRAVELS_API FWordProgress
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
    bool bDiscovered = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
    int32 CorrectAnswers = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
    int32 Mistakes = 0;
};