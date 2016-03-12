// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "UModComboBox.h"

TSharedRef<SWidget> UUModComboBox::HandleGenerateWidget(TSharedPtr<FString> Item) const
{
	FString StringItem = Item.IsValid() ? *Item : FString();

	// Call the user's delegate to see if they want to generate a custom widget bound to the data source.
	if (!IsDesignTime() && OnGenerateWidgetEvent.IsBound())
	{
		UWidget* Widget = OnGenerateWidgetEvent.Execute(StringItem);
		if (Widget != NULL)
		{
			return Widget->TakeWidget();
		}
	}
	
	//UMod ComboBoxString Customize injector start
	TSharedRef<STextBlock> widget = SNew(STextBlock).Text(FText::FromString(StringItem));

	if (!ElementsTooltipText.IsEmpty()) {
		FString rep = ElementsTooltipText.Replace(TEXT("%e"), **Item, ESearchCase::IgnoreCase);
		widget->SetToolTipText(FText::FromString(rep));
	}	
	
	FTextBlockStyle fuckUE4Const = ElementsTextStyle;
	FTextBlockStyle *style = new FTextBlockStyle(ElementsTextStyle);	
	widget->SetTextStyle(style);
	return widget;
	//UMod ComboBoxString Customize injector end
}

UUModComboBox::UUModComboBox(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{	
}