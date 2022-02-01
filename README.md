# PatrickShimabukuro_Escape

The objective of this project was to create an escape game

## It followed 4 steps:
- 1- Created a New Level and made a house with 2 doors and a few objects in it using static meshes and importing some textures
- 2- Created a "Grabber" component and attached it to my Default_Pawn BluePrint used to make my actor grab an object
- 3- Created a Pressure Plate in which the player can place objects triggering the doors to open
- 4- Created an "OpenDoor" component and attached it to my door meshes in order to make them open when the objects were placed in the pressure plate

This readme file will point the some of the most important parts of the code. The complete code is in its separate files.

## 1- Creating a New Level

This is the level I created using box brushes for the walls, floors and ceiling, a subtractive box brush to make a whole in the wall to place the exit door, door, cube and cone meshes as objects and surfance materials and textures downloaded from texture.com and modified on Gimp to .jpg format and then Uasset to be read by Unreal Engine.d

![image](https://user-images.githubusercontent.com/12215115/151789894-9fba1a15-c4a4-431a-a882-831f26c479d8.png)
![image](https://user-images.githubusercontent.com/12215115/151789991-2fedec59-011f-4b6b-a88b-295235c4bf56.png)

## 2- Creating a "Grabber"

On BeggingPlay:
Set up the physics handle which is a component that allows the actor to attach another object to or close to his body, like if it were holding it.


```cpp
// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandle();
	SetUpInputComponent();
}

// Check if physics handle is working
void UGrabber::FindPhysicsHandle()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

	if (!PhysicsHandle)
	{
		// Physics handle is not found
		UE_LOG(LogTemp, Error, TEXT("Physics Handle not found in component: %s"), *GetOwner()->GetName());
	}
}
```

Set up the input component:
  - The SetUpInnputComponent needs first to be set in the project settings on Unreal: Engine > Input

![image](https://user-images.githubusercontent.com/12215115/151792021-02d7ea99-dd1d-47eb-84d9-af8cc7252c56.png)

  - Then it needs to be called in the code passing the parameters for the input event (button to be pressed), defining which object is calling it and the function that this input will perform - whether grab or realease, from the UGrabber function.

```cpp
// Set action for pressed and release grab functions
void UGrabber::SetUpInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	} 
}

```
If the input is on, it will call the UGrabber::Grab() function:

The grab function will then call the GetFirstPhysicsBodyInReach() that queries which objects around the actor are being reached and assigns the result to a FHitResult variable
Once we know what we hit, we then assign the component hit to a UPrimitiveComponent* variable using the GetComponent() function
Then if we hit a component from the type AActor, then it will use our physics handle variable to grab our component using its GrabComponentArLocation function (it takes in a UPrimitiveComponent*)

```cpp
void UGrabber::Grab()
{
	FHitResult HitResult = GetFirstPhysicsBodyInReach();

	// The component to be grabbed
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
```
GetFirstPhysicsBodyInReach uses the LineTracingSingleByObjectType function that "draws" a line from the player to the object like an invisible arm to allow the player to grab something at a predefined distance.

```cpp
FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
	FHitResult Hit;

	// Crate a var to store the parameters of the collision query itself
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	// Ray casting for the player to reach out something at a certain distance (Reach)
	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		GetPlayersWorldPos(),
		GetPlayersReach(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody), 
		TraceParams
	);

	return Hit;
}

```
LineTracingByObject type needs to know where the player is located in the world - GetPlayerWorldPosition() - and what is the extension of the players invisible arm - GetPlayersReach()

The player's position is defined by 2 variables: its location (FVerctor variable) and where he is looking to, or its rotation (FRotator variable) and we get those by accessing GetPlayerViewPoint()

```cpp
FVector UGrabber::GetPlayersWorldPos() const
{
	// Get Players view point
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	// Update PlayerViewPointLocation and Rotation variables with the current value
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
		);
	
	return PlayerViewPointLocation;
}
```
To define the players arm's reach we do the same thing but we return then a line that extends from the player's location until a defined distance (reach) following a certain direction defined by a vector:

```cpp
// Returns line trace end
FVector UGrabber::GetPlayersReach() const
{
	// Get Players view point
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	//OUT just indicates that these are out parameters. out parameters is when you pass a 
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
		);
	
	// Players view point location. to which distance from the player the grabber object will be located?
	return PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
}
```
The release input will use the ReleaseComponent() function from the physics handle

```cpp
void UGrabber::Release()
{
	// Nullptr protection. if there is not a pointer object PhysicsHandle, quit the iteration and return
	if(!PhysicsHandle){return;}
	PhysicsHandle->ReleaseComponent();
}
```

The players reach will be updated in every tick

```cpp
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
```

## 3- Pressure Plate

The Pressure Plate is the place where the player needs to place the 2 objects is order for the doors to open

First we need to include a trigger volume object in our world and this will be our pressure plate

![image](https://user-images.githubusercontent.com/12215115/151966769-726905e6-55a6-4edf-a1f9-7f271d801aca.png)

Then we need to create a OpenDoor component and instantiate a ATriggerVolume* variable in our OpenDoor header file to represent our pressure plate object in our code and expose this variable to make it editable anywhere

```cpp
UPROPERTY(EditAnywhere)
ATriggerVolume* PressurePlate = nullptr; // use * because is a pointer
```

Then we add the OpenDoor component to our doors in Unreal and click on OpenDoor component and assign our PressurePlate variable in the Pressure Plate slot

![image](https://user-images.githubusercontent.com/12215115/151967892-18802931-a2dc-4096-8cd7-8dbff32c3ece.png)

Now we need to define what triggers the trigger volume that we called PressurePlate by creating a float variable MassToOpenDoors that will be used as the limit for the total mass of all the actors in that volume that is needed in order to trigger the OpenDoor function. This will also be exposed to edit:

```cpp
UPROPERTY(EditAnywhere)
float MassToOpenDoors = 50.f;
```











