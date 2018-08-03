// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QTreeTester.generated.h"


UCLASS()
class AQTreeTester : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQTreeTester();

	UPROPERTY(EditAnywhere)
	TArray<class AActor *> testPoints;

	class QTree *tree;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	
	void TestAddingPoints();

	void TestAutoAddingSpawnPoints();

	void TestRandomSpawning();

	void AssertArrayEqual(TArray<class AActor*> arr1, TArray<class AActor*> arr2)
	{

	}
	void AssertArrayMatch()
	{

	}
	
};
