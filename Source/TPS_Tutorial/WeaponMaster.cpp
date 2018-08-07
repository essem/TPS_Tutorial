// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponMaster.h"
#include "TPS_TutorialCharacter.h"
#include "UnrealNetwork.h"

AWeaponMaster::AWeaponMaster()
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);

	MuzzleSocket = FName("Muzzle");
}

void AWeaponMaster::ServerStartFire_Implementation()
{
	StartFire();
}

bool AWeaponMaster::ServerStartFire_Validate()
{
	return true;
}

void AWeaponMaster::ServerStopFire_Implementation()
{
	StopFire();
}

bool AWeaponMaster::ServerStopFire_Validate()
{
	return true;
}

void AWeaponMaster::StartFire()
{
	if (!HasAuthority())
	{
		ServerStartFire();
	}
	
	if (!bWantsToFire)
	{
		bWantsToFire = true;
	}

	CameraAim();
}

void AWeaponMaster::StopFire()
{
	if (!HasAuthority())
	{
		ServerStopFire();
	}
		
	if (bWantsToFire)
	{
		bWantsToFire = false;
	}
}

FVector AWeaponMaster::GetMuzzleLocation() const
{
	return WeaponMesh->GetSocketLocation(MuzzleSocket);
}

void AWeaponMaster::CameraAim()
{
	AimVector = OwningPawn->GetBaseAimRotation().Vector();

	static const FName LineTraceSingleName(TEXT("TPS_Tutorial_LineTraceSingle"));
	//GetWorld()->DebugDrawTraceTag = LineTraceSingleName;

	FCollisionQueryParams Params;
	Params.TraceTag = LineTraceSingleName;
	Params.AddIgnoredActor(this);

	FVector Start = GetMuzzleLocation();
	FVector End;
	FHitResult HitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, Params);

	if (bHit)
	{
		AimVector = HitResult.Location;
	}

	AimVector = OwningPawn->GetBaseAimRotation().Vector();
}

void AWeaponMaster::SetOwningPawn(ATPS_TutorialCharacter* InOwningPawn)
{
	if (OwningPawn != InOwningPawn)
	{
		OwningPawn = InOwningPawn;
		SetOwner(OwningPawn);
	}
}

void AWeaponMaster::OnRep_OwningPawn()
{
	if (OwningPawn && bNeedsAttachedUpdateOnOwnerRep)
	{
		bNeedsAttachedUpdateOnOwnerRep = false;

		if (!HasAuthority())
		{
			if (OwningPawn->GetEquippedWeapon() != this && OwningPawn->GetHolstedWeapons(SlotType) == this)
			{
				AttachToOwnerHolster();
			}
		}
	}
}

void AWeaponMaster::AttachToOwnerHolster()
{
	if (OwningPawn)
	{
		WeaponMesh->AttachToComponent(OwningPawn->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, OwningPawn->GetRHandWeaponSocket());
		OwningPawn->SetEquippedWeapon(this);
	}
	else
	{
		bNeedsAttachedUpdateOnOwnerRep = true;
	}
}

void AWeaponMaster::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponMaster, OwningPawn);
}
