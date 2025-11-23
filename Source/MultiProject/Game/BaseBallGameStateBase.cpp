// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BaseBallGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BaseBallPlayerController.h"

void ABaseBallGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	if (HasAuthority() == false)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PC) == true)
		{
			ABaseBallPlayerController* BaseBallPC = Cast<ABaseBallPlayerController>(PC);
			if (IsValid(BaseBallPC) == true)
			{
				FString NotificationString = InNameString + TEXT(" has joined the game.");
				BaseBallPC->PrintChatMessageString(NotificationString);
			}
		}
	}
}
