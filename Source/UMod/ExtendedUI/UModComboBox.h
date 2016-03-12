// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ComboBoxString.h"
#include "UModComboBox.generated.h"

/**
 * 
 */
UCLASS(Meta = (DisplayName = "UModComboBox (String)"))
class UMOD_API UUModComboBox : public UComboBoxString
{
	GENERATED_BODY()
		
public:
	UUModComboBox(const FObjectInitializer& ObjectInitializer);

	/**
	 * Text style to use for styling element's texts blocks.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UMod_Specific)
		FTextBlockStyle ElementsTextStyle;

	/**
	 * Elements tool-tip text, all "%e" occurences are replaced by the element content.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UMod_Specific)
		FString ElementsTooltipText;

	virtual TSharedRef<SWidget> HandleGenerateWidget(TSharedPtr<FString> Item) const;
	
};
