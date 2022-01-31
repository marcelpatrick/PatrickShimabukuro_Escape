// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Grabber.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDING_ESCAPE_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabber();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
float Reach = 100.f;

// Physics handle is an object for moving physics objects around
	// * because it is a pointer to a UPhysics handle component
	// needs to be initialized pointing to a null pointer because when we initiate the game we won't know
	//if the physics handle has spawn up before the grabber. If it doesn't it will crash. 
	//and initiallizing as a null pointer prevents this
	// And everything that has a "U" on the beginning needs to have a Uproperty function before so that the macros can
	//clean things up for us
	UPROPERTY()
UPhysicsHandleComponent* PhysicsHandle = nullptr; 
UPROPERTY()
UInputComponent* InputComponent = nullptr;

void Grab();
void Release();
void FindPhysicsHandle();
void SetUpInputComponent();

// Return first actor within reach with a physics body
FHitResult GetFirstPhysicsBodyInReach() const;

// Return line trace end
FVector GetPlayersReach() const;
		
// Get Players position in the world
FVector GetPlayersWorldPos() const;

};
