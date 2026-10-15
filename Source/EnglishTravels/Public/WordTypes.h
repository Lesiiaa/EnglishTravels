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