// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TPS_TutorialCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "TPS_TutorialPlayerController.h"
#include "UnrealNetwork.h"
#include "WeaponMaster.h"

//////////////////////////////////////////////////////////////////////////
// ATPS_TutorialCharacter

ATPS_TutorialCharacter::ATPS_TutorialCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Overlap);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->SocketOffset = FVector(0, 55, 55);
	CameraBoom->TargetArmLength = 100.0f;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	CameraVisibilitySphere = CreateDefaultSubobject<USphereComponent>(TEXT("CameraVisibilitySphere"));
	CameraVisibilitySphere->SetSphereRadius(12.0f);
	CameraVisibilitySphere->RelativeLocation = FVector(-10.0f, 0.0f, 0.0f);
	CameraVisibilitySphere->SetupAttachment(FollowCamera);


	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	RHandWeaponSocket = FName("RHand_WeaponSocket");
}

void ATPS_TutorialCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ATPS_TutorialCharacter::BeginPlay()
{
	Super::BeginPlay();

	GiveDefaultWeapons();
}

void ATPS_TutorialCharacter::GiveDefaultWeapons()
{
	AWeaponMaster* WeaponMaster = nullptr;
	for (auto WeaponClass : DefaultWeapons)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		WeaponMaster = GetWorld()->SpawnActor<AWeaponMaster>(WeaponClass, GetMesh()->GetComponentToWorld(), SpawnParams);
		if (WeaponMaster)
		{
			WeaponMaster->SetOwningPawn(this);
		}
	}

	if (WeaponMaster)
	{
		WeaponMaster->AttachToOwnerHolster();
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATPS_TutorialCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATPS_TutorialCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATPS_TutorialCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATPS_TutorialCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATPS_TutorialCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ATPS_TutorialCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ATPS_TutorialCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ATPS_TutorialCharacter::OnResetVR);

	PlayerInputComponent->BindAction("AimDownSights", IE_Pressed, this, &ATPS_TutorialCharacter::OnAimDownSights);
	PlayerInputComponent->BindAction("AimDownSights", IE_Released, this, &ATPS_TutorialCharacter::OnAimDownSights);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ATPS_TutorialCharacter::OnAttackPressed);
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &ATPS_TutorialCharacter::OnAttackReleased);
}


void ATPS_TutorialCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ATPS_TutorialCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ATPS_TutorialCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void ATPS_TutorialCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATPS_TutorialCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATPS_TutorialCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ATPS_TutorialCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ATPS_TutorialCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	bool bOverlapped = GetCapsuleComponent()->IsOverlappingComponent(CameraVisibilitySphere);
	GetMesh()->SetOwnerNoSee(bOverlapped);

	float RightTraceValue = IKFootTrace(IKTraceDistance, RightFootSocket);
	float CurrentRightOffset = FMath::FInterpTo(IKOffsetRightFoot, RightTraceValue, DeltaSeconds, IKInterpSpeed);

	if (CurrentRightOffset < 40.0f)
	{
		IKOffsetRightFoot = CurrentRightOffset;
	}

	float LeftTraceValue = IKFootTrace(IKTraceDistance, LeftFootSocket);
	float CurrentLeftOffset = FMath::FInterpTo(IKOffsetLeftFoot, LeftTraceValue, DeltaSeconds, IKInterpSpeed);

	if (CurrentLeftOffset < 40.0f)
	{
		IKOffsetLeftFoot = CurrentLeftOffset;
	}
}

AWeaponMaster* ATPS_TutorialCharacter::GetHolstedWeapons(EWeaponInventorySlot Slot) const
{
	switch (Slot)
	{
	case EWeaponInventorySlot::Melee:
		return MeleeWeapon;

	case EWeaponInventorySlot::Primary:
		return PrimaryWeapon;

	case EWeaponInventorySlot::Secondary:
		return SecondaryWeapon;
	}

	return nullptr;
}

ATPS_TutorialPlayerController* ATPS_TutorialCharacter::GetCastedOwner() const
{
	return Cast<ATPS_TutorialPlayerController>(GetController());
}

void ATPS_TutorialCharacter::OnAimDownSights()
{
	ToggleADS();

	if (bPlayerIsADS)
	{
		CameraBoom->TargetArmLength = 60.0f;
		FollowCamera->SetFieldOfView(75.0f);
	}
	else
	{
		CameraBoom->TargetArmLength = 100.0f;
		FollowCamera->SetFieldOfView(90.0f);
	}
}

void ATPS_TutorialCharacter::OnAttackPressed()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->StartFire();
	}
}

void ATPS_TutorialCharacter::OnAttackReleased()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->StopFire();
	}
}

void ATPS_TutorialCharacter::ToggleADS()
{
	if (!bPlayerIsADS)
	{
		StartADS();
	}
	else
	{
		StopADS();
	}
}

void ATPS_TutorialCharacter::StartADS()
{
	if (!HasAuthority())
	{
		ServerStartADS();
	}

	bPlayerIsADS = true;

	FollowCamera->bUsePawnControlRotation = true;
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

void ATPS_TutorialCharacter::StopADS()
{
	if (!HasAuthority())
	{
		ServerStopADS();
	}

	bPlayerIsADS = false;

	FollowCamera->bUsePawnControlRotation = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

float ATPS_TutorialCharacter::IKFootTrace(float TraceDistance, FName Socket)
{
	FVector SocketLocation = GetMesh()->GetSocketLocation(Socket);
	FVector ActorLocation = GetActorLocation();

	float ToBottomOfFoot = TraceDistance - ActorLocation.Z;

	FVector Start(SocketLocation.X, SocketLocation.Y, ActorLocation.Z);
	FVector End(SocketLocation.X, SocketLocation.Y, ToBottomOfFoot);

	static const FName LineTraceTag(TEXT("IKFootTrace"));

	FCollisionQueryParams Params;
	Params.TraceTag = LineTraceTag;
	Params.AddIgnoredActor(this);

	FHitResult HitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

	float IKOffset = 0.0f;
	if (bHit)
	{
		IKOffset = (HitResult.Location - End).Size() / Scale;
	}

	return IKOffset;
}

void ATPS_TutorialCharacter::ServerStartADS_Implementation()
{
	StartADS();
}

bool ATPS_TutorialCharacter::ServerStartADS_Validate()
{
	return true;
}

void ATPS_TutorialCharacter::ServerStopADS_Implementation()
{
	StopADS();
}

bool ATPS_TutorialCharacter::ServerStopADS_Validate()
{
	return true;
}

void ATPS_TutorialCharacter::OnRep_EquippedWeapon()
{
}

void ATPS_TutorialCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATPS_TutorialCharacter, bPlayerIsADS);
	DOREPLIFETIME(ATPS_TutorialCharacter, EquippedWeapon);
}
