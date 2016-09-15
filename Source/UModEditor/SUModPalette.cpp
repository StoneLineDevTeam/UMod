#include "UModEditor.h"
#include "SUModPalette.h"

void SUModPalette::Construct(const FArguments& Args)
{
	/*ChildSlot
		[
			//Creating the button that adds a new item on the list when pressed
			SNew(SScrollBox)
			+ SScrollBox::Slot()
		[

	//The actual list view creation
	+ SScrollBox::Slot()
		[
			SAssignNew(ListViewWidget, SListView<TSharedPtr<FString>>)
			.ItemHeight(24)
		.ListItemsSource(&Items) //The Items array is the source of this listview
		.OnGenerateRow(this, &SUModPalette::OnGenerateRowForList)
		]
		];*/
}

FReply SUModPalette::ButtonPressed()
{
	//Adds a new item to the array (do whatever you want with this)
	Items.Add(MakeShareable(new FString("Hello 1")));

	//Update the listview
	ListViewWidget->RequestListRefresh();

	return FReply::Handled();
}


TSharedRef<ITableRow> SUModPalette::OnGenerateRowForList(TSharedPtr<FString> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	//Create the row
	return
		SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
		.Padding(2.0f)
		[
			SNew(SButton).Text(FText::FromString(*Item.Get()))
		];
}