// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TPS_TutorialPlayerController.generated.h"

UCLASS()
class ATPS_TutorialPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	FVector GetViewPoint() const;
	FVector GetAimVector() const;

private:
	FVector OutViewLocation = FVector::ZeroVector;
};
