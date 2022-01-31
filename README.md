# PatrickShimabukuro_Escape

The objective of this project was to create an escape game

##It followed 3 steps:
- 1- Created a New Level and made a house with 2 doors and a few objects in it using static meshes and importing some textures
- 2- Created a "Grabber" component and attached it to my Default_Pawn BluePrint used to make my actor grab an object
- 3- Created a Pressure Plate in which the player can place objects triggering the doors to open
- 4- Created an "OpenDoor" component and attached it to my door meshes in order to make them open when the objects were placed in the pressure plate


##1- Creating a New Level

This is the level I created using box brushes for the walls, floors and ceiling, a subtractive box brush to make a whole in the wall to place the exit door, door, cube and cone meshes as objects and surfance materials and textures downloaded from texture.com and modified on Gimp to .jpg format and then Uasset to be read by Unreal Engine.d

![image](https://user-images.githubusercontent.com/12215115/151789894-9fba1a15-c4a4-431a-a882-831f26c479d8.png)
![image](https://user-images.githubusercontent.com/12215115/151789991-2fedec59-011f-4b6b-a88b-295235c4bf56.png)

##2- Creating a "Grabber"

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Grabber.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDING_ESCAPE_API UGrabber : public UActorComponent

```
