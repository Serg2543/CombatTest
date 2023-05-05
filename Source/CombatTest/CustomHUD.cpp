// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomHUD.h"

#include "CombatTestCharacter.h"

void ACustomHUD::DrawHUD()
{
	if (bStartSelecting)
	{
		CurrentPoint = GetMousePos2D();
		DrawRect(FLinearColor(0, 0, 1, 0.15f), InitialPoint.X, InitialPoint.Y, CurrentPoint.X - InitialPoint.X, CurrentPoint.Y - InitialPoint.Y);
		for (int i = 0; i < SelectedActorsDragged.Num(); i++)
		{
			SelectedActorsDragged[i]->SetSelected(false);
		}
		SelectedActorsDragged.Empty();
		GetActorsInSelectionRectangle<ACombatTestCharacter>(InitialPoint, CurrentPoint, SelectedActorsDragged, false, false);
		for (int i = 0; i < SelectedActorsDragged.Num(); i++)
		{
			SelectedActorsDragged[i]->SetSelected(true);
		}
	}
}

FVector2D ACustomHUD::GetMousePos2D()
{
	float PosX, PosY;
	GetOwningPlayerController()->GetMousePosition(PosX, PosY);
	return FVector2D(PosX, PosY);
}