// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/BaseBallPawn.h"
#include "MultiProject/MultiProject.h"

void ABaseBallPawn::BeginPlay()
{
	Super::BeginPlay();

	FString NetRoleString = MultiProjectFunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("CXPawn::BeginPlay() %s [%s]"), *MultiProjectFunctionLibrary::GetNetModeString(this), *NetRoleString);
	MultiProjectFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}

void ABaseBallPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	FString NetRoleString = MultiProjectFunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("CXPawn::PossessedBy() %s [%s]"), *MultiProjectFunctionLibrary::GetNetModeString(this), *NetRoleString);
	MultiProjectFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}
