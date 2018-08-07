// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TPS_TutorialPlayerController.h"
#include "TPS_TutorialCharacter.h"
#include "Camera/CameraComponent.h"

FVector ATPS_TutorialPlayerController::GetViewPoint() const
{
	ATPS_TutorialCharacter* Character = Cast<ATPS_TutorialCharacter>(GetPawn());
	if (Character)
	{
		UCameraComponent* FollowCamera = Character->GetFollowCamera();
		if (FollowCamera)
		{
			FMinimalViewInfo DesiredView;
			FollowCamera->GetCameraView(0.0f, DesiredView);
			return DesiredView.Location;
		}
	}

	return GetFocalLocation();
}

FVector ATPS_TutorialPlayerController::GetAimVector() const
{
	APawn* Pawn = GetPawn();

	if (Pawn)
	{
		return Pawn->GetBaseAimRotation().Vector();
	}
	else
	{
		return GetControlRotation().Vector();
	}
}
