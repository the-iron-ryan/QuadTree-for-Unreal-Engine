#pragma once

#include "Engine.h"

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, text)
#define printColor(text, fcolor) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, fcolor, text)
#define printf(text, fstring) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, FString::Printf(TEXT(text), fstring))
#define printfColor(text, fcolor, fstring) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, fcolor, FString::Printf(TEXT(text), fstring))

