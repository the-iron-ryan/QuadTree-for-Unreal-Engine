// Copyright (c) 2018 Ryan Dougherty. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"

/**
 * Quadrant represents an area of space within a 2D X,Y plane
 */
enum Quadrant
{
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight,
	Outside
};

/**
 * QTree is a basic implementation of a generic C++ Quad Tree for UE4. 
 * Works for all subclasses of AActor so that it can properly get all position data and sort it accordingly
 */
class QTree
{
public:
	/**
	 * Default constructor for empty tree with no defined boundaries
	 */
	QTree(int bucketSize = 3) : data(), bucket_size(bucketSize)
	{
		this->topLeftBounds = FVector2D::ZeroVector;
		this->bottomRightBounds = FVector2D::ZeroVector;

		for (int i = 0; i < 4; i++)
			trees[i] = NULL;
	}

	/**
	 * Constructor for empty QTree specifying boundaries
	 * 
	 * @param StartBounds Starting top left boundary
	 * @param EndBounds End bottom right boundary 
	 */
	QTree(FVector2D StartBounds, FVector2D EndBounds, int bucketSize = 3) : data(), bucket_size(bucketSize)
	{
		this->topLeftBounds = StartBounds;
		this->bottomRightBounds = EndBounds;

		for (int i = 0; i < 4; i++)
			trees[i] = NULL;
	}

	/**
	 * Constructor for QTree with array of Actor points as input
	 * 
	 * @param Actors list of actors to add to the QTree
	 */
	QTree(TArray<AActor*> Actors, int bucketSize = 3) : data(), bucket_size(bucketSize)
	{
		FVector2D *smallest = NULL;
		FVector2D *biggest = NULL;

		// Find the biggest and smallest points in the list
		for (AActor *act : Actors)
		{
			FVector actorLocation = act->GetActorLocation();
			FVector2D actorLocation2D = FVector2D(actorLocation.X, actorLocation.Y);
			if (!smallest || actorLocation2D < *smallest)
				smallest = &actorLocation2D;
			if (!biggest || actorLocation2D > *biggest)
				biggest = &actorLocation2D;
		}

		// Set the end boundaries
		SetBounds(*smallest, *biggest);

		// Add all Actors to the tree
		for (AActor *act : Actors)
		{
			this->Add(act);
		}
	}

	/**
	 * Default destructor
	 */
	~QTree()
	{
		for (int i = 0; i < 4; i++)
			if (trees[i] != NULL)
			{
				delete trees[i];
				trees[i] = NULL;
			}
	}

	/**
	 * Adds an actor to the QTree
	 * 
	 * @param Act Actor to be added into the quad tree
	 */
	FORCEINLINE bool Add(AActor *Act)
	{
		// Get the quadrant this point lies in
		FVector actorLocation = Act->GetActorLocation();
		Quadrant quad = GetQuadrant(FVector2D(actorLocation.X, actorLocation.Y), topLeftBounds, bottomRightBounds);

		// Bounds checking
		if (quad == Quadrant::Outside)
		{
			if (bCanExpandBounds)
			{
				ExpandBounds(FVector2D(actorLocation.X, actorLocation.Y), topLeftBounds, bottomRightBounds);
				return Add(Act);
			}
			else
			{
				return false;
			}
		}

		// If there is enough space in this tree, add node to the Act list
		if (data.Num() < bucket_size)
		{
			data.Add(Act);
			return true;
		}

		FVector2D midPoint = GetMidpoint(topLeftBounds, bottomRightBounds);

		// Check the positioning of the quadrant and determine which tree it should be added to
		switch (quad)
		{
		case TopLeft:
			if (topLeftTree == NULL)
			{
				topLeftTree = new QTree(topLeftBounds, midPoint);
				topLeftTree->bCanExpandBounds = this->bCanExpandBounds;
			}

			return topLeftTree->Add(Act);
			break;

		case TopRight:
			if (topRightTree == NULL)
			{
				topRightTree = new QTree(FVector2D(midPoint.X, topLeftBounds.Y), FVector2D(bottomRightBounds.X, midPoint.Y));
				topRightTree->bCanExpandBounds = this->bCanExpandBounds;
			}

			return topRightTree->Add(Act);
			break;
			
		case BottomLeft:
			if (bottomLeftTree == NULL)
			{
				bottomLeftTree = new QTree(FVector2D(topLeftBounds.X, midPoint.Y), FVector2D(midPoint.X, bottomRightBounds.Y));
				bottomLeftTree->bCanExpandBounds = this->bCanExpandBounds;
			}

			return bottomLeftTree->Add(Act);
			break;

		case BottomRight:
			if (bottomRightTree == NULL)
			{
				bottomRightTree = new QTree(midPoint, bottomRightBounds);
				bottomRightTree->bCanExpandBounds = this->bCanExpandBounds;
			}

			return bottomRightTree->Add(Act);
			break;

		default:
			return false;
		}
		return false;
	}

	/**
	 * Adds an actor to the QTree
	 * 
	 * @param Actors List of all actors to add to the tree
	 */
	FORCEINLINE bool Add(const TArray<AActor*> &Actors)
	{
		bool status = true;
		for (AActor* act : Actors)
		{
			if (!this->Add(act))
				status = false;
		}
		return status;
	}

	/**
	 * Removes an actor to the QTree
	 *
	 * @param Position Position of Actor to remove from the tree
	 * @returns True if successfully removes an Actor in the tree at the given position
	 */
	FORCEINLINE bool Remove(FVector2D Position)
	{
		// Search through this tree's data for the actor with the matching position
		for (AActor *act : data)
		{
			FVector actLocation = act->GetActorLocation();
			if (Position.Equals(FVector2D(actLocation.X, actLocation.Y)))
			{
				data.Remove(act);
				return true;
			}
		}

		// Search underlying trees for matching Nodes
		Quadrant quad = GetQuadrant(Position, topLeftBounds, bottomRightBounds);
		switch (quad)
		{
		case TopLeft:
			if (topLeftTree == NULL)
				return false;
			else
				return topLeftTree->Remove(Position);
			break;
		case TopRight:
			if (topRightTree == NULL)
				return false;
			else
				return topRightTree->Remove(Position);
			break;
		case BottomLeft:
			if (bottomLeftTree == NULL)
				return false;
			else
				return bottomLeftTree->Remove(Position);
			break;
		case BottomRight:
			if (bottomRightTree == NULL)
				return false;
			else
				return bottomRightTree->Remove(Position);
			break;
		case Outside:
			return false;
			break;
		}
		return false;
	}

	/**
	 * Finds an actor in the QTree based of off 2D position
	 *
	 * @param Position Position of Actor to find in the tree
	 * @returns Actor in tree at given position
	 */
	FORCEINLINE AActor * Find(FVector2D Position)
	{
		// Search through this tree's data and find the matching point
		for (int i = 0; i < bucket_size; i++)
		{
			FVector actPos = data[i]->GetActorLocation();
			if (Position.Equals(FVector2D(actPos.X, actPos.Y)))
				return data[i];
		}

		// Search underlying trees for matching Nodes
		Quadrant quad = GetQuadrant(Position, topLeftBounds, bottomRightBounds);
		switch (quad)
		{
		case TopLeft:
			if (topLeftTree == NULL)
				return NULL;
			else
				return topLeftTree->Find(Position);
			break;
		case TopRight:
			if (topRightTree == NULL)
				return NULL;
			else
				return topRightTree->Find(Position);
			break;
		case BottomLeft:
			if (bottomLeftTree == NULL)
				return NULL;
			else
				return bottomLeftTree->Find(Position);
			break;
		case BottomRight:
			if (bottomRightTree == NULL)
				return NULL;
			else
				return bottomRightTree->Find(Position);
			break;
		case Outside:
			return NULL;
			break;
		}
		return NULL;
	}

	/**
	 * Finds an actor in the tree closest to the desired position
	 *
	 * @param Position Position closest to the nearest Actor in the tree
	 * @returns Actor in tree at nearest position
	 */
	FORCEINLINE AActor * FindNearest(FVector2D Position)
	{
		AActor *nearest = NULL;
		float closestDist;
		QTree *currentTree = this;

		// Continue iterating until there are no more trees
		while (currentTree != NULL)
		{
			AActor* n = currentTree->GetNearestActorInData(Position);
			if (n == NULL)
				break;

			FVector actLocation = n->GetActorLocation();
			float distance = FVector2D::Distance(Position, FVector2D(actLocation.X, actLocation.Y));

			// Set the closest distance iff nearest point is not assigned or current distance is less than closest so far
			if (nearest == NULL || distance < closestDist)
			{
				nearest = n;
				closestDist = distance;
			}

			// Pick the next tree to look into
			Quadrant quad = GetNearestQuadrant(Position, currentTree->topLeftBounds, currentTree->bottomRightBounds);
			switch (quad)
			{
			case TopLeft:
				currentTree = currentTree->topLeftTree;
				break;
			case TopRight:
				currentTree = currentTree->topRightTree;
				break;
			case BottomLeft:
				currentTree = currentTree->bottomLeftTree;
				break;
			case BottomRight:
				currentTree = currentTree->bottomRightTree;
				break;
			case Outside:
				currentTree = NULL;
				break;
			}
		}
		return nearest;
	}

	/**
	 * Gets the boundaries of the tree
	 *
	 * @returns A size 2 array of the top left boundary position (index 0) and bottom right boundary position (index 1)
	 */
	FORCEINLINE FVector2D * GetBounds()
	{
		return new FVector2D[2]{ topLeftBounds, bottomRightBounds };
	}

	/**
	 * Sets the boundary of the QTree with a newly given top left and bottom right position
	 *
	 * @param TopLeft The top left position in the given tree
	 * @param BottomRight The bottom right position in the tree
	 */
	FORCEINLINE void SetBounds(FVector2D TopLeft, FVector2D BottomRight)
	{
		SetBoundsRecursively(TopLeft, BottomRight);
		Rebalance();
	}

	/**
	 * Sets the boundary of the QTree with an array of 2D points
	 *
	 * @param Bounds Size 2 array of top left boundary position (index 0) and bottom right boundary position (index 1) 
	 */
	FORCEINLINE void SetBounds(FVector2D Bounds[2])
	{
		SetBounds(Bounds[0], Bounds[1]);
	}

	/**
	 * Returns an array of all Actors in the tree
	 *
	 * @returns An array of all of the Actors in the tree
	 */
	FORCEINLINE TArray<class AActor*> GetAllActors()
	{
		return Traverse();
	}

	/**
	 * Returns whether or not the tree has child trees 
	 *
	 * @returns True if the tree has any child trees
	 */
	FORCEINLINE bool HasChildren()
	{
		for (QTree *tree : trees)
		{
			if (tree != NULL)
				return true;
		}
		return false;
	}

	/**
	 * Determines whether or not the tree and all its children will recalculate their boundaries when SetBounds is called
	 */
	bool bCanExpandBounds = true;

private:

	/**
	 * Gets the midpoint between two points
	 * 
	 * @params FirstPoint first vector
	 * @params SecondsPoint second vector
	 * @returns The midpoint vector between two vectors
	 */
	FVector2D GetMidpoint(FVector2D startBounds, FVector2D endBounds) const
	{
		return FVector2D((startBounds.X + endBounds.X) / 2, (startBounds.Y + endBounds.Y) / 2);
	}

	/**
	 * Given a position and its boundary, this will give the quadrant the point lies inclusive to the boundary space
	 * 
	 * @params Position Vector to be classified in a quadrant
	 * @params TopLeft Top left boundary point
	 * @params BottomRight Bottom right boundary point
	 * @returns A quadrant inclusive to the boundary space
	 */
	Quadrant GetQuadrant(FVector2D pos, FVector2D topLeft, FVector2D bottomRight) const
	{
		FVector2D midPoint = GetMidpoint(topLeft, bottomRight);
		if (pos.X >= topLeft.X && pos.X <= midPoint.X) // Within left of midpoint
		{
			if (pos.Y >= topLeft.Y && pos.Y <= midPoint.Y)
				return Quadrant::TopLeft;
			else if (pos.Y > midPoint.Y && pos.Y <= bottomRight.Y)
				return Quadrant::BottomLeft;
		}
		else if (pos.X > midPoint.X && pos.X <= bottomRight.X) // Within right of midpoint
		{
			if (pos.Y >= topLeft.Y && pos.Y <= midPoint.Y)
				return Quadrant::TopRight;
			else if (pos.Y > midPoint.Y && pos.Y <= bottomRight.Y)
				return Quadrant::BottomRight;
		}
		return Quadrant::Outside;
	}

	/**
	 * Given a position and its boundary, this will give the quadrant the point lies exclusive to the boundary space
	 * 
	 * @params Position Vector to be classified in a quadrant
	 * @params TopLeft Top left boundary point
	 * @params BottomRight Bottom right boundary point
	 * @returns A quadrant exclusive to the boundary space
	 */
	Quadrant GetNearestQuadrant(FVector2D pos, FVector2D topLeft, FVector2D bottomRight) const
	{
		FVector2D midPoint = GetMidpoint(topLeft, bottomRight);
		if (pos.X <= midPoint.X) // Within left of midpoint
		{
			if (pos.Y <= midPoint.Y)
				return Quadrant::TopLeft;
			else
				return Quadrant::BottomLeft;
		}
		else
		{
			if (pos.Y <= midPoint.Y)
				return Quadrant::TopRight;
			else
				return Quadrant::BottomRight;
		}

	}

	/**
	 * Traverses the tree in pre-order traversed form
	 *
	 * @returns A list of Actors currently in the tree
	 */
	TArray<class AActor*> Traverse()
	{
		TArray<AActor *> actors;

		// Append this tree's data first
		actors.Append(data);

		// Traverse each child tree and append their respective data
		for (QTree *tree : trees)
		{
			if (tree != NULL)
				actors.Append(tree->Traverse());
		}
		return actors;
	}

	/**
	 * Traverses the tree in pre-order traversed form and pops all values out of it until the tree is completely cleared
	 *
	 * @returns A list of Actors were in the tree
	 */
	TArray<class AActor*> TraverseAndPop()
	{
		TArray<AActor *> actors;

		// Append this tree's data first
		actors.Append(data);
		data.Empty();

		// Traverse each child tree and append their respective data
		for (QTree *tree : trees)
		{
			if (tree != NULL)
				actors.Append(tree->TraverseAndPop());
		}
		return actors;
	}

	/**
	 * Recursively sets the bounds of the given tree and all of its child trees
	 * 
	 * @param TopLeft The top left position in the given tree
	 * @param BottomRight The bottom right position in the tree
	 */
	void SetBoundsRecursively(FVector2D TopLeft, FVector2D BottomRight)
	{
		this->topLeftBounds = TopLeft;
		this->bottomRightBounds = BottomRight;
		FVector2D mid = GetMidpoint(topLeftBounds, bottomRightBounds);

		// Set bounds for new trees
		if (topLeftTree)
			topLeftTree->SetBoundsRecursively(topLeftBounds, mid);
		if (topRightTree)
			topRightTree->SetBoundsRecursively(FVector2D(mid.X, topLeftBounds.Y), FVector2D(bottomRightBounds.X, mid.Y));
		if (bottomLeftTree)
			bottomLeftTree->SetBoundsRecursively(FVector2D(topLeftBounds.X, mid.Y), FVector2D(mid.X, bottomRightBounds.Y));
		if (bottomRightTree)
			bottomRightTree->SetBoundsRecursively(mid, bottomRightBounds);
	}

	/**
	 * Pops and adds back all of the Actors onto the tree to rebalance it
	 */
	void Rebalance()
	{
		TArray<AActor *> allActs = this->TraverseAndPop();
		for (AActor *act : allActs)
		{
			this->Add(act);
		}
	}

	/*
	 * Expands the boundaries of the tree
	 *
	 * @param Position The position of the point to expand relative to
	 * @param TopLeft The top left position in this tree
	 * @param BottomRight The bottom right position in this tree
	 */
	void ExpandBounds(FVector2D Position, FVector2D TopLeft, FVector2D BottomRight)
	{
		if (!bCanExpandBounds)
			return;

		FVector2D newTopLeft = TopLeft;
		FVector2D newBottomRight = BottomRight;

		if (Position.X < TopLeft.X)
			newTopLeft.X = Position.X;
		if (Position.X > BottomRight.X)
			newBottomRight.X = Position.X;
		if (Position.Y < TopLeft.Y)
			newTopLeft.Y = Position.Y;
		if (Position.Y > BottomRight.Y)
			newBottomRight.Y = Position.Y;

		this->SetBounds(newTopLeft, newBottomRight);


	}
	/*
	 * Gets the actor nearest to the given position relatively only to the individual trees data
	 * 
	 * @params Position Vector to find the Actor located closest to
	 * @returns An Actor nearest to the passed in position
	 */
	class AActor * GetNearestActorInData(FVector2D pos)
	{
		float closestDist;
		AActor *nearest = NULL;

		for (AActor *act : data)
		{
			FVector actPos = act->GetActorLocation();
			float distance = FVector2D::Distance(pos, FVector2D(actPos.X, actPos.Y));
			if (nearest == NULL || distance < closestDist)
			{
				nearest = act;
				closestDist = distance;
			}
		}
		return nearest;
	}

private:
	/** Bucket size for this tree */
	const int bucket_size = 3;

	/** Boundary points for the node */
	FVector2D topLeftBounds;
	FVector2D bottomRightBounds;

	/** Data stored in this QTree */
	TArray<class AActor*> data;

	/** Child nodes accessible by array indexing or direct access */
	union
	{
		struct
		{
			QTree *topLeftTree;
			QTree *topRightTree;
			QTree *bottomLeftTree;
			QTree *bottomRightTree;
		};

		QTree *trees[4];
	};
};
