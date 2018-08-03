// Copyright 2018 Ryan Dougherty. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spawner.generated.h"

UCLASS(BlueprintType, Blueprintable,meta=(ShortTooltip="Spawns a given class at the nearest spawn point location."))
class ASpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	/**
	 * Default constructor for the spawner
	 */
	ASpawner();

	/**
	 * Spawns the given actor subclass at a location nearest to the one passed in
	 *
	 * @param Location Nearest position to spawn the object
	 * @param ActorToSpawn Actor subclass to spawn
	 * @param SpawnMethod Collision behavior when spawning the object
	 *
	 * @returns Spawned Actor object reference
	 */
	AActor* SpawnAtNearestLocation(FVector2D Location, TSubclassOf<AActor> ActorToSpawn);

	/**
	 * Spawns the given actor subclass at a location nearest to the one passed in
	 *
	 * @param Location Nearest position to spawn the object
	 * @param ActorToSpawn Actor subclass to spawn
	 * @param SpawnMethod Collision behavior when spawning the object
	 *
	 * @returns Spawned Actor object reference
	 */
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SpawnAtNearestLocation(FVector2D Location, TSubclassOf<AActor> ActorToSpawn, UPARAM(DisplayName="Spawned Actor") AActor* &SpawnedActor_out, ESpawnActorCollisionHandlingMethod SpawnMethod = ESpawnActorCollisionHandlingMethod::Undefined);

	/**
	 * Spawns an actor at a random location from the list of possible spawn points
	 *
	 * @param ActorToSpawn Actor subclass to spawn
	 * @param SpawnMethod Collision behavior when spawning the object
	 *
	 * @returns Spawned Actor object reference
	 */
	AActor* SpawnAtRandomLocation(TSubclassOf<AActor> ActorToSpawn);

	/**
	 * Spawns an actor at a random location from the list of possible spawn points
	 *
	 * @param ActorToSpawn Actor subclass to spawn
	 * @param SpawnMethod Collision behavior when spawning the object
	 *
	 * @returns Spawned Actor object reference
	 */
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SpawnAtRandomLocation(TSubclassOf<AActor> ActorToSpawn, UPARAM(DisplayName="Spawned Actor") AActor* &SpawnedActor_out, ESpawnActorCollisionHandlingMethod SpawnMethod = ESpawnActorCollisionHandlingMethod::Undefined);

	/**
	 * Gets all active spawn points currently a part of this spawner
	 *
	 * @returns A list of active spawn points
	 */
	UFUNCTION(BlueprintCallable)
	TArray<AActor *> GetAllSpawnPoints();


	/**
	 *Called every frame
	 */
	virtual void Tick(float DeltaTime) override;

protected:
	/**
	 * Called when the game starts or when spawned
	 */
	virtual void BeginPlay() override;

public:
	/** List of all spawn points manually added */
	UPROPERTY(EditAnywhere, Category = "Spawning Options")
	TArray<AActor *> SpawnPoints;

	/** If true, automatically gets all objects inherited from ASpawnPoint in the level and adds them as possible spawn points*/
	UPROPERTY(EditAnywhere, Category = "Spawning Options")
	bool bAutoAddAllSpawnPoints;

private:
	/** Underlying QTree structure to store all of spawn points */
	class QTree *tree;
};
