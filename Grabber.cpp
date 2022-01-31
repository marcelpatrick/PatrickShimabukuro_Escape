// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

#define OUT // #define lets us use certain words in our code that don't do anything but are
//just used to mark out our code. like comments but inside the actual code.

// ACESSORS AND MEMORY
// : Accesses functions or data that are saved in Permament Storage memory - where we store things that 
//are permanent and don't change and thus, is known at compile time - known quantities
//Ex: Class, Enum, Namespace 
// . and -> Access functions or data that are saved in Temporary Storage memory (Stack and Heap) - where we store things that
//change at run time
//Ex: Instance or Reference (.) to access a function of that instance
// and Pointer (->) that points to an address in memory to access members of that class

// NULL POINTER PROTECTION
// For every pointer variable or object, always initialize it as "nullptr" - which is like initializing an int = 0
// This will ensure that that object is always pointing to some value an prevent us from a null pointer error
// A null pointer error is when we try to use a variable that doesn't exist or doesn't have any value asigned to it 
//not even zero or "nullptr"
// we don't need to worry about pointers withing function such as GetOwner() or GetWorld() because these will aways have a 
//component attached. (if we are working on code that doesn't have a world to ir then we don't have anything to work on...)

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	// Check for physics handdle component
		// <> because it is a function template. <specify the class>
		// function templates can be used with any kind of variable or class so that you don't need to create
		//the same function to deal with each type of variable or class
		// Ex: GetMax<int>(x, y) or GetMax<long>(x, y)
		// FindComponentByClass attributes to this variable a component of that specified class
	FindPhysicsHandle();
	SetUpInputComponent();
}

// Check if physics handle is working
void UGrabber::FindPhysicsHandle()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

	// If there is nothing asigned to PhysicsHandle then log the error 
		//(otherwise it would give us a nullptr error) which is when we try to use an object that doesn't have any value asigned to it
		//not even zero or "nullptr"
	if (!PhysicsHandle)
	{
		// Physics handle is not found
		UE_LOG(LogTemp, Error, TEXT("Physics Handle not found in component: %s"), *GetOwner()->GetName());
	}
}

// Set action for pressed and release grab functions
void UGrabber::SetUpInputComponent()
{
	// Get an input component (when player presses anything) and asign this component to a variable
	// we don't need to protect this poiter because any component that is on an actor is going to have an owner
	//so we don't run the risk of pointing to a component that doesn't exist or was not initialized - what would give us a 
	//"nullpointer" error
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		// -> because it is a pointer variable
		// BindAction is going to bind the action of pressing a button to this variable, as we set up in the project settings
			// "Grab" is the name of the action that we created
			// IE_Pressed is the input event
			// "this": defines the object. it is a pointer to "this" current object 
			// & is the address of the grab function that is part of the UGrabber Class 
			//(the function that we are going to use for this action)
			// &Grabber::Grab does not have () because we are not calling the function but just referring to its address
			//(or the place it is located in memory so that the function know where to find it)
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	} 
}

void UGrabber::Grab()
{
	// TODO to only raycast when key is pressed and see if we reach any actors with physics body collision channel set
	FHitResult HitResult = GetFirstPhysicsBodyInReach();

	// The component to be grabbed
	// PrimitiveComponents are SceneComponents that contain or generate some sort of geometry
	//eg. ShapeComponents (Capsule, Sphere, Box), StaticMeshComponent, and SkeletalMeshComponent.
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();

	AActor* ActorHit = HitResult.GetActor();

	// If hit something then attach physics handle
	if (ActorHit)
	{	
		// Nullptr protection. if there is not a pointer object PhysicsHandle, quit the iteration and return
		if (!PhysicsHandle){return;}
	
		// Attach physics handle
		PhysicsHandle->GrabComponentAtLocation
			(
			ComponentToGrab,
			// Name for the bone to which the object will be attached. since we don't have bones = None
			NAME_None,
			// End of the line trace
			GetPlayersReach()
			);
	}
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
	FHitResult Hit;

	// Crate a var to store the parameters of the collision query itself
	// parameters:
		// Leaving FName as an empty text because we don't have one but we still need to fill in with something
		// false because we are not using complex collision
		// ignore the player itself so that the collision doesn't hit it. so ignore the owner.
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	// Ray casting for the player to reach out something at a certain distance (Reach)
	// FCollisionObjectQueryParams(EcollisionChannel - indicates which body channel we are using here. 
	//in this case, a Physics Body) and it taks in a 
	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		GetPlayersWorldPos(),
		GetPlayersReach(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody), 
		TraceParams
	);

	return Hit;
}

FVector UGrabber::GetPlayersWorldPos() const
{
	// Get Players view point
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	// Update PlayerViewPointLocation and Rotation variables with the current value
		//OUT just indicates that these are out parameters. out parameters is when you pass a 
		//parameter in your function that is going to be modified by that function
		//is different from the return in which, with return, you take the parameters as they come
		//to perform a function and the result of the funtion is the return value
		//with out parameters you take these parameters are modified themselves in a function
		//also, return is only able to return one function. with out parameters you can get out
		//of the function as many parameters as the function takes
		//so functions with out parameters don't need return because they return the parameters themselves
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
		);
	
	return PlayerViewPointLocation;
}

// Returns line trace end
FVector UGrabber::GetPlayersReach() const
{
	// Get Players view point
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	//OUT just indicates that these are out parameters. out parameters is when you pass a 
		//parameter in your function that is going to be modified by that function
		//is different from the return in which, with return, you take the parameters as they come
		//to perform a function and the result of the funtion is the return value
		//with out parameters you take these parameters are modified themselves in a function
		//also, return is only able to return one function. with out parameters you can get out
		//of the function as many parameters as the function takes
		//so functions with out parameters don't need return because they return the parameters themselves
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
		);
	
	// Players view point location. to which distance from the player the grabber object
		//will be located?
	return PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
}

void UGrabber::Release()
{
	// Nullptr protection. if there is not a pointer object PhysicsHandle, quit the iteration and return
	if(!PhysicsHandle){return;}
	PhysicsHandle->ReleaseComponent();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// If physics handle is attached 
	// Nullptr protection. if there is not a pointer object PhysicsHandle, quit the iteration and return
	if(!PhysicsHandle){return;}
	if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(GetPlayersReach());
	}
}

