// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h" 
#include "InputActionValue.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"
#include "CollisionQueryParams.h"
#include "PlayerCharacter.generated.h"


UCLASS()
class PROJECT_PZ_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// (  edited by property windows , can be read by Blueprints, but not modified , can be found under inputs in BP   )
#pragma region Inputs
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> PlayerMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> SprintAction;

	
#pragma endregion Inputs

#pragma region Player Data
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Replicated, Category = "Input")
	float HeadPitch;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Health",Replicated)
	int32 Health;

	UPROPERTY(BlueprintReadOnly,Category="Data")
	float SprintSpeed = 500.0f;

	UPROPERTY(BlueprintReadOnly,Category="Data")
	float WalkingSpeed = 250.0f;

	UPROPERTY(BlueprintReadOnly,Category="Data")
	bool Sprinting = false;

	UPROPERTY(BlueprintReadOnly,Category="Data",Replicated)
	int32 Direction = 0;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Interaction")
	float InteractRange = 1000.0f;


	
#pragma endregion Player Data
	
#pragma region Replicateted Functions
	
#pragma region Replicateted Functions Notes
	/*Reliable
	 Will arrive
	 Arrives in oder
	 Will retry until its confirmed delivered 
	use for critical game State changes
	use on things like firing a weapon, reloading, equipping an item match start and end 
	*/
	/*Unreliable
	use this for things that are sent every frame and that are not critical and does not matter what order it arrives in
	Is not retired if the packet is dropped
	*/
#pragma  endregion eplicateted Functions Notes
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(Server,Unreliable)
	void Server_SetHeadPitch(float pitch_);
	void Server_SetHeadPitch_Implementation(float pitch_);

	UFUNCTION(Server,Unreliable)
	void Server_SetDirection(float dir_);
	void Server_SetDirection_Implementation(float dir_);

	UFUNCTION(Server,Reliable)
	void Server_OnInteract(AActor* hitActor_);
	void Server_OnInteract_Implementation(AActor* hitActor_);
#pragma endregion Replicateted Functions 
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

#pragma region Input Functions 
	// Input action functions 
	UFUNCTION()
	void Move(const FInputActionValue& Value);
	
	UFUNCTION()
	void Look(const FInputActionValue& Value);
	
	UFUNCTION()
	void Interact();

	UFUNCTION()
	void StartSprint();

	UFUNCTION()
	void EndSprint();
#pragma endregion Input Functions

#pragma region Camera Info	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraFOV = 70.0f;

	UPROPERTY(EditAnywhere, Category = Camera)
	float FirstPersonScale = 0.6f;

	// Offset for the first-person camera
	UPROPERTY(EditAnywhere, Category = Camera)
	FVector FirstPersonCameraOffset = FVector(2.8f, 5.9f, 0.0f);
#pragma endregion Camera Info

#pragma region player Mesh
	// First-person mesh, visible only to the owning player
	UPROPERTY(VisibleAnywhere, Category = Mesh)
	TObjectPtr<USkeletalMeshComponent> FirstPersonMeshComponent;
#pragma endregion player Mesh
};



