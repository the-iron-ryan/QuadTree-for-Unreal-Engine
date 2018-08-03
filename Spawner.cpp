// Fill out your copyright notice in the Description page of Project Settings.

#include "Spawner.h"
#include "Helpers.h"
#include "QTree.h"
#include "SpawnPoint.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Public/EngineUtils.h"


// Sets default values
ASpawner::ASpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	bAutoAddAllSpawnPoints = true;
	tree = new QTree();
	tree->bCanExpandBounds = true;
}


AActor* ASpawner::SpawnAtNearestLocation(FVector2D Location, TSubclassOf<AActor> ActorToSpawn)
{
	AActor *spawnedAct = NULL;
	AActor *nearestSpawnPoint = tree->FindNearest(Location);
	FActorSpawnParameters params;

	if (nearestSpawnPoint)
		spawnedAct = GetWorld()->SpawnActorAbsolute(ActorToSpawn, nearestSpawnPoint->GetActorTransform(), params);
	else
		UE_LOG(LogTemp, Error, TEXT("No spawn point in tree found"));

	return spawnedAct;
}

void ASpawner::SpawnAtNearestLocation(FVector2D Location, TSubclassOf<AActor> ActorToSpawn, AActor* &SpawnedActor_out, ESpawnActorCollisionHandlingMethod SpawnMethod)
{
	AActor *spawnedAct = NULL;
	AActor *nearestSpawnPoint = tree->FindNearest(Location);
	FActorSpawnParameters params;

	params.SpawnCollisionHandlingOverride = SpawnMethod;

	if (nearestSpawnPoint)
		spawnedAct = GetWorld()->SpawnActorAbsolute(ActorToSpawn, nearestSpawnPoint->GetActorTransform(), params);
	else
		UE_LOG(LogTemp, Error, TEXT("No spawn point in tree found"));

	SpawnedActor_out = spawnedAct;
}
AActor* ASpawner::SpawnAtRandomLocation(TSubclassOf<AActor> ActorToSpawn)
{
	AActor *spawnedAct = NULL;
	TArray<AActor *> AllSpawnPoints = tree->GetAllActors();

	if (AllSpawnPoints.Num() < 1)
	{
		UE_LOG(LogTemp, Error, TEXT("No spawn point in tree found"));
		return NULL;
	}


	int size = AllSpawnPoints.Num();
	int randIndex = FMath::RandRange(0, size - 1);

	FActorSpawnParameters params;

	spawnedAct = GetWorld()->SpawnActorAbsolute(ActorToSpawn, AllSpawnPoints[randIndex]->GetActorTransform(), params);

	return spawnedAct;
}
void ASpawner::SpawnAtRandomLocation(TSubclassOf<AActor> ActorToSpawn, AActor* &SpawnedActor_out, ESpawnActorCollisionHandlingMethod SpawnMethod)
{
	AActor *spawnedAct = NULL;
	TArray<AActor *> AllSpawnPoints = tree->GetAllActors();

	if (AllSpawnPoints.Num() < 1)
	{
		UE_LOG(LogTemp, Error, TEXT("No spawn point in tree found"));
		SpawnedActor_out = NULL;
		return;
	}


	int size = AllSpawnPoints.Num();
	int randIndex = FMath::RandRange(0, size - 1);

	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = SpawnMethod;

	spawnedAct = GetWorld()->SpawnActorAbsolute(ActorToSpawn, AllSpawnPoints[randIndex]->GetActorTransform(), params);

	SpawnedActor_out = spawnedAct;
}

// Called when the game starts or when spawned
void ASpawner::BeginPlay()
{
	// Only add custom spawn points if the list is filled
	if(SpawnPoints.Num() > 0)
		tree->Add(SpawnPoints);

	// Add all spawn points placed in level
	if (bAutoAddAllSpawnPoints)
	{
		for (TActorIterator<ASpawnPoint> actItr(GetWorld()); actItr; ++actItr)
		{
			tree->Add(*actItr);
		}
	}

	Super::BeginPlay();
}

TArray<AActor*> ASpawner::GetAllSpawnPoints()
{
	return tree->GetAllActors();
}

// Called every frame
void ASpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

