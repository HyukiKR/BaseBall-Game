#include "Player/BaseBallPlayerController.h"
#include "UI/BaseBallChatInput.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MultiProject/MultiProject.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Game/BaseBallGameModeBase.h"
#include "Player/BaseBallPlayerState.h"
#include "Net/UnrealNetwork.h"

ABaseBallPlayerController::ABaseBallPlayerController()
{
	bReplicates = true;
}

void ABaseBallPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}
	
	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	if (IsValid(ChatInputWidgetClass) == true)
	{
		ChatInputWidgetInstance = CreateWidget<UBaseBallChatInput>(this, ChatInputWidgetClass);
		if(IsValid(ChatInputWidgetInstance) == true)
		{
			ChatInputWidgetInstance->AddToViewport();
		}
	}

	if (IsValid(NotificationTextWidgetClass) == true)
	{
		NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass);
		if (IsValid(NotificationTextWidgetInstance) == true)
		{
			NotificationTextWidgetInstance->AddToViewport();
		}
	}
}

void ABaseBallPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;

	if (IsLocalController() == true)
	{
		ABaseBallPlayerState* BaseBallPS = GetPlayerState<ABaseBallPlayerState>();
		if (IsValid(BaseBallPS) == true)
		{
			FString CombinedMessageString = BaseBallPS->GetPlayerInfoString() + TEXT(": ") + InChatMessageString;

			ServerRPCPrintChatMessageString(CombinedMessageString);
		}
	}
}

void ABaseBallPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	MultiProjectFunctionLibrary::MyPrintString(this, InChatMessageString, 10.f);
}

void ABaseBallPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotificationText);
}

void ABaseBallPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);
}

void ABaseBallPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	if (IsValid(GM) == true)
	{
		ABaseBallGameModeBase* BaseBallGM = Cast<ABaseBallGameModeBase>(GM);
		if (IsValid(BaseBallGM) == true)
		{
			BaseBallGM->PrintChatMessageString(this, InChatMessageString);
		}
	}
}
