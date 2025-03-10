#pragma once
#include "SlateBasics.h"

/*class SUModPaletteButton : public SButton {

};*/

class SUModPalette : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(SUModPalette) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& Args);
	FReply ButtonPressed();

	/* Adds a new textbox with the string to the list */
	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FString> Item, const TSharedRef<STableViewBase>& OwnerTable);

	/* The list of strings */
	TArray<TSharedPtr<FString>> Items;

	/* The actual UI list */
	TSharedPtr<SListView<TSharedPtr<FString>>> ListViewWidget;
};
