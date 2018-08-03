// Copyright (c) 2018 Ryan Dougherty. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"

/**
 * Octant represents an area of space within a 3D X, Y, Z plane
 */
enum Octant
{
	TopLeftBack,
	TopRightBack,
	BottomLeftBack,
	BottomRightBack,
	TopLeftFront,
	TopRightFront,
	BottomLeftFront,
	BottomRightFront
};

class OctTree
{
public:
	/**
	 * Default constructor for empty OctTree with no defined boundaries
	 */
	OctTree() : data()
	{
		this->topLeftFrontBounds = FVector::ZeroVector;
		this->bottomRightBackBounds = FVector::ZeroVector;

		for (int i = 0; i < 8; i++)
			trees[i] = NULL;
	}

private:
	/** Bucket size for this tree */
	static const int bucket_size = 3;

	/** Boundary points for the node */
	FVector topLeftFrontBounds;
	FVector bottomRightBackBounds;

	/** Data stored in this QTree */
	TArray<class AActor*> data;

	/** Child nodes accessible by array indexing or direct access */
	union
	{
		struct
		{
			OctTree *topLeftBackTree;
			OctTree *topRightBackTree;
			OctTree *bottomLeftBackTree;
			OctTree *bottomRightBackTree;

			OctTree *topLeftFrontTree;
			OctTree *topRightFrontTree;
			OctTree *bottomLeftFrontTree;
			OctTree *bottomRightFrontTree;
		};

		OctTree *trees[8];
	};

};