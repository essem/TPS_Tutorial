// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponMaster.h"

AWeaponMaster::AWeaponMaster()
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);
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

void AWeaponMaster::CameraAim()
{
}
