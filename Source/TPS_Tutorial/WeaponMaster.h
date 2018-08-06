// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponMaster.generated.h"

UENUM(BlueprintType)
enum class EWeaponInventorySlot : uint8
{
	None,
	Melee,
	Primary,
	Secondary
};

USTRUCT(BlueprintType)
struct FCharacterWeaponSlot
{
	GENERATED_BODY()
		
	EWeaponInventorySlot WeaponSlot;
	FName HolsterSlot;
};

UCLASS()
class TPS_TUTORIAL_API AWeaponMaster : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponMaster();

	void StartFire();
	void StopFire();

private:
	void CameraAim();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartFire();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopFire();

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* WeaponMesh;

	bool bWantsToFire = false;
};
