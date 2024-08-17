// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(Aura, Log, All);

#define AuraDEBUG_CALLINFO (FString(__FUNCTION__) + TEXT(": ") + FString::FromInt(__LINE__))
#define AuraLOG_S(Verbosity) UE_LOG(Aura, Verbosity, TEXT("%s"), *AuraDEBUG_CALLINFO)
#define AuraLOG(Verbosity, Format, ...) UE_LOG(Aura, Verbosity, TEXT("%s %s"), *AuraDEBUG_CALLINFO, *FString::printf(Format, ##__VA_ARGS__))




