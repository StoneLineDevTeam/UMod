// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/Widget.h"
#include "Frame.generated.h"

/**
 * 
 */
UCLASS()
class UFrame : public UWidget
{
	GENERATED_BODY()

public:
	void SetPos(FVector2D vec);
	FVector2D GetPos();
	void SetSize(int w, int h);
	void Add(UWidget *child);
	void SetColor(FColor col);
private:
	int SizeX;
	int SizeY;

	UButton *CloseBut;
	UImage *Icon;
	UImage *Background;

	UPROPERTY(VisibleAnywhere)
	UPanelSlot *FrameMainPanel;
};
