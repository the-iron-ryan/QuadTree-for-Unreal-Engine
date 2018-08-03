// Fill out your copyright notice in the Description page of Project Settings.

#include "QTreeTester.h"
#include "QTree.h"
#include "Helpers.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"


// Sets default values
AQTreeTester::AQTreeTester() : testPoints()
{
	tree = new QTree(FVector2D(-100, -100), FVector2D(100, 100));
	tree->bCanExpandBounds = true;

	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AQTreeTester::BeginPlay()
{
	Super::BeginPlay();

	TestAutoAddingSpawnPoints();
}

// Called every frame
void AQTreeTester::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AQTreeTester::TestAddingPoints()
{	
	
	for (AActor *act : testPoints)
	{
		if (!tree->Add(act))
			print("Could not add.");
	}
	TArray<AActor *> arr = tree->GetAllActors();
	for (AActor *act : arr)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *act->GetActorLocation().ToString());
	}

	tree->SetBounds(FVector2D(-100, -100), FVector2D(500, 500));

	TArray<AActor *> arr2 = tree->GetAllActors();

	UE_LOG(LogTemp, Warning, TEXT("Tree after boundry change"));
	for (AActor *act : arr2)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *act->GetActorLocation().ToString());
	}
}

void AQTreeTester::TestAutoAddingSpawnPoints()
{
	UE_LOG(LogTemp, Warning, TEXT("List of auto added points"));

	TArray<AActor *> arr = tree->GetAllActors();
	for (AActor *act : arr)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *act->GetActorLocation().ToString());
	}
}

void AQTreeTester::TestRandomSpawning()
{

}

