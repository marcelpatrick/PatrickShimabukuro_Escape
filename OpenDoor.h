// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TriggerVolume.h"
#include "OpenDoor.generated.h" 


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDING_ESCAPE_API UOpenDoor : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOpenDoor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void OpenDoor(float DeltaTime);
	void CloseDoor(float DeltaTime);
	float TotalMassOfActors() const; 
	void FindAudioComponent();
	void FindpressurePlate();

	// to ensure the sound is only played once at a time. tracks whether the sound has been played
	bool OpenDoorSound = true;
	bool CloseDoorSound = true;

private:

float InitialYaw;
float CurrentYaw;

float DoorLastOpened = 0.f;

//Expose value to the editor - set each door individually to open up
UPROPERTY(EditAnywhere)
float OpenAngle = 90.f;

UPROPERTY(EditAnywhere)
float DoorCloseDelay = .5f;
		
UPROPERTY(EditAnywhere)
float DoorCloseSpeed = .8f;

UPROPERTY(EditAnywhere)
float DoorOpenSpeed = 2.f;

UPROPERTY(EditAnywhere)
ATriggerVolume* PressurePlate = nullptr; // use * because is a pointer

UPROPERTY(EditAnywhere)
float MassToOpenDoors = 50.f;

UPROPERTY()
UAudioComponent* AudioComponent = nullptr; 

};
