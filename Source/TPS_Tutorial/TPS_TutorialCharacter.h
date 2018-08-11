// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WeaponInventorySlot.h"
#include "TPS_TutorialCharacter.generated.h"

UCLASS(config=Game)
class ATPS_TutorialCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	class USphereComponent* CameraVisibilitySphere = nullptr;

public:
	ATPS_TutorialCharacter();
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	virtual void Tick(float DeltaSeconds) override;

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable)
	bool IsPlayerADS() const { return bPlayerIsADS; }

	FName GetRHandWeaponSocket() const { return RHandWeaponSocket; }

	class AWeaponMaster* GetEquippedWeapon() const { return EquippedWeapon; }
	void SetEquippedWeapon(class AWeaponMaster* InEquippedWeapon) { EquippedWeapon = InEquippedWeapon; }

	class AWeaponMaster* GetHolstedWeapons(EWeaponInventorySlot Slot) const;

	class ATPS_TutorialPlayerController* GetCastedOwner() const;

private:
	void GiveDefaultWeapons();

	void OnAimDownSights();
	void OnAttackPressed();
	void OnAttackReleased();

	void ToggleADS();
	void StartADS();
	void StopADS();

	float IKFootTrace(float TraceDistance, FName Socket);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartADS();
	void ServerStartADS_Implementation();
	bool ServerStartADS_Validate();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopADS();
	void ServerStopADS_Implementation();
	bool ServerStopADS_Validate();

	UFUNCTION()
	void OnRep_EquippedWeapon();

	UPROPERTY(Replicated)
	bool bPlayerIsADS = false;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	class AWeaponMaster* EquippedWeapon;

	UPROPERTY(Replicated)
	class AWeaponMaster* MeleeWeapon;

	UPROPERTY(Replicated)
	class AWeaponMaster* PrimaryWeapon;

	UPROPERTY(Replicated)
	class AWeaponMaster* SecondaryWeapon;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class AWeaponMaster>> DefaultWeapons;

	UPROPERTY(EditAnywhere)
	FName RHandWeaponSocket;

	// Animation Handlers : IK Feet
	float IKTraceDistance = 0.0f;
	FName RightFootSocket = FName("foot_rSocket");
	FName LeftFootSocket = FName("foot_lSocket");
	float Scale = 100.0f;
	float IKOffsetRightFoot = 0.0f;
	float IKOffsetLeftFoot = 0.0f;
	float IKInterpSpeed = 10.0f;
};
