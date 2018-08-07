// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponInventorySlot.h"
#include "WeaponMaster.generated.h"

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

	// Pawn
	void SetOwningPawn(class ATPS_TutorialCharacter* InOwningPawn);
	void AttachToOwnerHolster();

private:
	FVector GetMuzzleLocation() const;
	void CameraAim();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartFire();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopFire();

	UFUNCTION()
	void OnRep_OwningPawn();

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere)
	FName MuzzleSocket;

	UPROPERTY(ReplicatedUsing = OnRep_OwningPawn)
	class ATPS_TutorialCharacter* OwningPawn;

	bool bWantsToFire = false;
	bool bNeedsAttachedUpdateOnOwnerRep = false;
	EWeaponInventorySlot SlotType = EWeaponInventorySlot::None;
	FVector AimVector = FVector::ZeroVector;
};
