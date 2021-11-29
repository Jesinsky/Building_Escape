// Copyright Jesus Moctezuma 2021

#include "Grabber.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

#define OUT

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

	FindPhysicsHandle();
	SetupInputComponent();
}

void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Input Component MISSING"), *GetOwner()->GetName());
	}
}

// Checking fo the Physics Hande Component
void UGrabber::FindPhysicsHandle()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("No Physics Handle Component found on %s"), *GetOwner()->GetName());
	}
}

void UGrabber::Grab()
{
	FHitResult HitResult = GetFisrtPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();

	if (HitResult.GetActor())
	{
		PhysicsHandle->GrabComponentAtLocation
			(
				ComponentToGrab,
				NAME_None,
				GetPlayersReach()
			);
	}
}


void UGrabber::Release()
{
	// UE_LOG(LogTemp, Warning, TEXT("Grabber Released"));
	PhysicsHandle->ReleaseComponent();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// If the physics handle is attach
	if (PhysicsHandle->GrabbedComponent)
	{
		// Move the object we are holding
		PhysicsHandle->SetTargetLocation(GetPlayersReach());
	}
	
}

FHitResult UGrabber::GetFisrtPhysicsBodyInReach() const
{

	FHitResult Hit;
	// Ray-cast out to certain distance (Reach)
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		GetPlayersWorldPosition(),
		GetPlayersReach(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);

	return Hit;
	// See what is hits
	// AActor* ActorHit = Hit.GetActor();

	// if (ActorHit)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Line trace has hit:%s"), *(ActorHit->GetName()));
	// }
	
	// return Hit;
}

FVector UGrabber::GetPlayersWorldPosition() const
{
	// Get players viewport
	FVector PlayerViewPointLocation;
	FRotator PlayerViePointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation, 
		OUT PlayerViePointRotation);

	// UE_LOG(LogTemp, Warning, TEXT("Position:%s, Rotation:%s"), 
	// 	*PlayerViewPointLocation.ToString(), 
	// 	*PlayerViePointRotation.ToString());

	return PlayerViewPointLocation;
	// Debug line to see the players reach distance
	/*DrawDebugLine(
		GetWorld(),
		PlayerViewPointLocation,
		LineTraceEnd,
		FColor(0, 255, 0),
		false,
		0.f,
		0,
		5.f
	 );*/
}

FVector UGrabber::GetPlayersReach() const
{
	// UE_LOG(LogTemp, Warning, TEXT("Grabber Pressed"));
	
	FVector PlayerViewPointLocation;
	FRotator PlayerViePointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation, 
		OUT PlayerViePointRotation);

	return PlayerViewPointLocation + PlayerViePointRotation.Vector() * Reach;
}