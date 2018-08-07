// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TPS_TutorialPlayerController.h"
#include "TPS_TutorialCharacter.h"
#include "Camera/CameraComponent.h"

void ATPS_TutorialPlayerController::ViewPoint()
{
	ATPS_TutorialCharacter* Character = Cast<ATPS_TutorialCharacter>(GetPawn());
	if (Character)
	{
		UCameraComponent* FollowCamera = Character->GetFollowCamera();
		if (FollowCamera)
		{
			FMinimalViewInfo DesiredView;
			FollowCamera->GetCameraView(0.0f, DesiredView);
			OutViewLocation = DesiredView.Location;
			return;
		}
	}

	OutViewLocation = GetFocalLocation();
}

void ATPS_TutorialPlayerController::AimVector()
{

}
