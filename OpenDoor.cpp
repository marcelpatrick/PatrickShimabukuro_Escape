// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDoor.h"
#include "Components/AudioComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"

#define OUT

// OWNER DEFINITION
	// Owner is who exerts control over the object we are referring to
	// eg: a player holding a flashlight: the PlayerController owns the Pawn (an actor without a body), the Pawn owns the 
	//Character (an actor with a body), the Character owns the flashlight
	// In order to do things to a specific actor we must first include a GetOwner() function in order to let the system
	//know which actor we are talking about. Then we can access this actors properties and functions using
	//GetOwner()->...functions 
	// it is a bottom up approach to find the actor that we are referring to 

// Diffent Lerp types for movement control of OpenDoor
	//float CurrentYaw = GetOwner()->GetActorRotation().Yaw;
	//FRotator OpenDoor(0.f, OpenAngle, 0.f);
	//Geometric progressions lerp. Door opens and gradually slows down
	//OpenDoor.Yaw = FMath::Lerp(CurrentYaw, OpenAngle, 0.02f);
	//True linear interpolator. door opens at a linear speed
	//OpenDoor.Yaw = FMath::FInterpConstantTo(CurrentYaw, OpenAngle, DeltaTime, 45);
	//Door opens at a linear speed but linearly slows down at the end
	//OpenDoor.Yaw = FMath::FInterpTo(CurrentYaw, OpenAngle, DeltaTime, 1);
	//GetOwner()->SetActorRotation(OpenDoor);

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	// Get door initial position to make it move 
		// GetOwner means which actor owns this function OpenDoor that I'm in. "Who is the owner of this function"
		// GetOwner() Get the owner of this Actor
		// it is a bottom up approach to find the actor that we are referring to 
	InitialYaw = GetOwner()->GetActorRotation().Yaw;
	CurrentYaw = InitialYaw;

	// Define to which angle the door will open
		// Updates the OpenAngle taking the default value set in the header file and adding the InitialYaw position
		//so that it starts rotating from the initial pivot point
			// += is the same as OpenAngle = OpenAngle + InitialYaw;
	OpenAngle += InitialYaw; 

	// Check functions to make sure we are beginning play with all components we need. otherwise, log and error.
	FindpressurePlate();
	FindAudioComponent();
}

void UOpenDoor::FindpressurePlate()
{
	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has no pressure plate"), *GetOwner()->GetName());
	}
}

void UOpenDoor::FindAudioComponent()
{
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();

	if (!AudioComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("%s missing audio component"), *GetOwner()->GetName());
	}
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Define condition to open door
		// if the total mass of the actors inside the pressure plate is greated than our limit variable, then we are going to open the door
		//Use of &&: if statements have lazy execution: when using && condition, if 1st is false, it won't execute the rest of the code (and in this case will crash)
		//In order to trick the system to execute at least part of the code (and avoid crashing), include a 1st condition that is always true
		//This avoids a "Null Pointer Error" in case I asign the WorldPosition component to an actor that does not have the PressurePlate in place
	if (TotalMassOfActors() > MassToOpenDoors) 
	{
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();

	} else {
		// if we passed the time for close delay, then close the door
		if (GetWorld()->GetTimeSeconds() - DoorLastOpened > DoorCloseDelay)
		{
			CloseDoor(DeltaTime);
		}	
	}
}

// Function to get the total mass of the actors placed on the open door area in order to 
//open the door with any actor placed there, not only the player
float UOpenDoor::TotalMassOfActors() const
{
	float TotalMass = 0.f;

	// Find overlapping actors, Store them in an array
	TArray<AActor*> OverlappingActors;

	// Go to the pressure plate to see whats in it. Get overlapping actors and store them in this array
		// Nullptr protection. it needs to return TotalMass because the whole function needs to return something
	if (!PressurePlate){return TotalMass;}
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);

	// Add their masses
	// Range based for loop simplification - use : instead of i < range, i++
		// (Define index variable type and give the index a name : array to interate)
	for (AActor* Actor : OverlappingActors)
	{
		// += adds up each iteration and reasigns to the updated variable value: 
			//the same as: updated TotalMass = previous TotalMass + Actor
			// PrimitiveComponents are SceneComponents that contain or generate some sort of geometry, 
			//generally to be rendered or used as collision
				// A Static Mesh for instance is a subclass of a PrimitiveComponent
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}
	return TotalMass;
}

void UOpenDoor::OpenDoor(float DeltaTime)
{
	//Rotate Door based on relative position
		//1- Update Current Yaw
		//2- Create a rotator variable
		//3- Update MyRotator's Yaw with the updated CurrentYaw
		//4- Pass my updated rotator as the new actor rotation position
	CurrentYaw = FMath::Lerp(CurrentYaw, OpenAngle, DeltaTime * DoorOpenSpeed); //DeltaTime to respect framerate speed. to change it, change 1.f to .5f for example
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation); 

	// Stop close door sound and if OpenDoorSound hasn't played it then play it	
	CloseDoorSound = false;
	if(!AudioComponent){return;}
	if (!OpenDoorSound)
	{
		AudioComponent->Play();
		OpenDoorSound = true;
	}
}

void UOpenDoor::CloseDoor(float DeltaTime)
{
	CurrentYaw = FMath::Lerp(CurrentYaw, InitialYaw, DeltaTime * DoorCloseSpeed);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);

	OpenDoorSound = false;
	if (!AudioComponent) {return;}
	if (!CloseDoorSound)
	{
		AudioComponent->Play();
		CloseDoorSound = true;
	}
}

