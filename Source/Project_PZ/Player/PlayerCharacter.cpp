// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h" 
#include "InputActionValue.h"
#include "PhysicsAssetRenderUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Project_PZ/Interfaces/InteractInterface.h"


class UInputMappingContext;
class UInputAction;
class UInputComponent;
// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// makes it so the player can not see their own face 

	GetMesh()->SetOnlyOwnerSee(true);

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	check(FirstPersonCameraComponent !=nullptr);

	FirstPersonMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	check(FirstPersonMeshComponent != nullptr);

	
	FirstPersonMeshComponent->SetupAttachment(GetMesh());
	FirstPersonMeshComponent->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMeshComponent->SetOnlyOwnerSee(false);
 
	// Set the first-person mesh to not collide with other objects
	FirstPersonMeshComponent->SetCollisionProfileName(FName("NoCollision"));
	
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMeshComponent,FName("HeadTop_EndSocket"));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = CameraFOV;
	FirstPersonCameraComponent->FirstPersonScale = FirstPersonScale;

	GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed;
	
	// Networking
	bReplicates = true;
	
	
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	//GetMesh()->SetOwnerNoSee(true);
	Super::BeginPlay();
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		
		// Get the enhanced input local player subsystem and add a new input mapping context to it
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerMappingContext, 0);
		}
	}

}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlayerCharacter,Health);
	DOREPLIFETIME(APlayerCharacter,HeadPitch);
	DOREPLIFETIME(APlayerCharacter,Direction);
}




// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered,this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered,this, &APlayerCharacter::Look);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered,this, &APlayerCharacter::Interact);
		EnhancedInputComponent->BindAction(SprintAction,ETriggerEvent::Triggered,this,&APlayerCharacter::StartSprint);
		EnhancedInputComponent->BindAction(SprintAction,ETriggerEvent::Completed,this,&APlayerCharacter::EndSprint);
	}
}
void APlayerCharacter::Interact()
{
	
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	FVector LineStart = FirstPersonCameraComponent->GetComponentLocation();
	FVector LineEnd = (FirstPersonCameraComponent->GetForwardVector()*InteractRange) + LineStart;
	DrawDebugLine(GetWorld(),LineStart,LineEnd,FColor::Red);
	if (GetWorld()->LineTraceSingleByChannel(HitResult,LineStart,LineEnd,ECC_Camera,CollisionParams))
	{
		if (HitResult.GetActor()->GetClass()->ImplementsInterface(UInteractInterface::StaticClass()))
		{
			Server_OnInteract(HitResult.GetActor());
		}
		UE_LOG(LogTemp,Display,TEXT("Name: %s"),*HitResult.GetActor()->GetName());
	}
	UE_LOG(LogTemp,Display,TEXT("Nothing"));
	
}

void APlayerCharacter::Server_OnInteract_Implementation(AActor* hitActor_)
{
	if (!hitActor_) return;
	IInteractInterface::Execute_OnInteract(hitActor_);
}

 void APlayerCharacter::Move(const FInputActionValue& Value)
{
	
	const FVector2d MoveValue = Value.Get<FVector2d>();

	if (Controller)
	{
		
		UE_LOG(LogTemp, Warning, TEXT("Walking"));
		const FVector Right = GetActorRightVector();
		AddMovementInput(Right,MoveValue.X);
		const FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward,MoveValue.Y);
		Direction = MoveValue.X;
		if (!HasAuthority())
		{
			Server_SetDirection(Direction);
		}
	}
}


void APlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2d LookValue = Value.Get<FVector2d>();

	if (Controller)
	{
		AddControllerYawInput(LookValue.X);
		AddControllerPitchInput(LookValue.Y);
		HeadPitch = FirstPersonCameraComponent->GetComponentRotation().Pitch;
		if (!HasAuthority())
		{
			Server_SetHeadPitch(HeadPitch);
		}
	}
	
}
void APlayerCharacter::StartSprint()
{
	if (Controller)
	{
		UE_LOG(LogTemp, Warning, TEXT("Start Sprint"));
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void APlayerCharacter::EndSprint()
{
	if (Controller)
	{
		UE_LOG(LogTemp, Warning, TEXT("End Sprint"));
		GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed;
	}
}

void APlayerCharacter::Server_SetHeadPitch_Implementation(float pitch_)
{
	HeadPitch = pitch_;
}

void APlayerCharacter::Server_SetDirection_Implementation(float dir_)
{
	Direction = dir_;
}