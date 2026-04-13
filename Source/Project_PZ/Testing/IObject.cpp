// Fill out your copyright notice in the Description page of Project Settings.


#include "IObject.h"

// Sets default values
AIObject::AIObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	
}

// Called when the game starts or when spawned
void AIObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AIObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

