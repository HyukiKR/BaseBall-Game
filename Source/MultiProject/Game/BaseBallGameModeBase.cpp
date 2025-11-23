// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BaseBallGameModeBase.h"
#include "Game/BaseBallGameStateBase.h"
#include "Player/BaseBallPlayerController.h"
#include "EngineUtils.h"
#include "Player/BaseBallPlayerState.h"

void ABaseBallGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	ABaseBallPlayerController* BaseBallPlayerController = Cast<ABaseBallPlayerController>(NewPlayer);
	if (IsValid(BaseBallPlayerController) == true)
	{
		BaseBallPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));

		AllPlayerControllers.Add(BaseBallPlayerController);

		ABaseBallPlayerState* BaseBallPS = BaseBallPlayerController->GetPlayerState<ABaseBallPlayerState>();
		if (IsValid(BaseBallPS) == true)
		{
			BaseBallPS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
		}

		ABaseBallGameStateBase* BaseBallGameStateBase = GetGameState<ABaseBallGameStateBase>();
		if (IsValid(BaseBallGameStateBase) == true)
		{
			BaseBallGameStateBase->MulticastRPCBroadcastLoginMessage(BaseBallPS->PlayerNameString);
		}
	}
}

FString ABaseBallGameModeBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for(int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num) {return Num > 0; });

	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	return Result;
}

bool ABaseBallGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	bool bCanPlay = false;

	do {
		if (InNumberString.Len() != 3)
		{
			break;
		}

		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			if (FChar::IsDigit(C) == false || C == '0')
			{
				bIsUnique = false;
				break;
			}

			UniqueDigits.Add(C);
		}

		if (bIsUnique == false)
		{
			break;
		}

		bCanPlay = true;
	} while (false);

	return bCanPlay;
}

FString ABaseBallGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if(InSecretNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if(InSecretNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%d(Strike) | %d(Ball)"), StrikeCount, BallCount);
}

void ABaseBallGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SecretNumberString = GenerateSecretNumber();
	UE_LOG(LogTemp, Error, TEXT("%s"), *SecretNumberString);
}

void ABaseBallGameModeBase::PrintChatMessageString(ABaseBallPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	int Index = InChatMessageString.Len() - 3;
	FString GuessNumberString = InChatMessageString.RightChop(Index);
	if (IsGuessNumberString(GuessNumberString) == true)
	{
		FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);

		IncreaseGuessCount(InChattingPlayerController);

		for (TActorIterator<ABaseBallPlayerController> It(GetWorld()); It; ++It)
		{
			ABaseBallPlayerController* BaseBallPlayerController = *It;
			if (IsValid(BaseBallPlayerController) == true)
			{
				FString CombinedMessageString = InChatMessageString + TEXT(" -> " + JudgeResultString);
				BaseBallPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);

				int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
				JudgeGame(InChattingPlayerController, StrikeCount);
			}
		}
	}
	else
	{
		for (TActorIterator<ABaseBallPlayerController> It(GetWorld()); It; ++It)
		{
			ABaseBallPlayerController* BaseBallPlayerController = *It;
			if (IsValid(BaseBallPlayerController) == true)
			{
				BaseBallPlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
			}
		}
	}
}

void ABaseBallGameModeBase::IncreaseGuessCount(ABaseBallPlayerController* InChattingPlayerController)
{
	ABaseBallPlayerState* BaseBallPS = InChattingPlayerController->GetPlayerState<ABaseBallPlayerState>();
	if (IsValid(BaseBallPS) == true)
	{
		BaseBallPS->CurrentGuessCount++;
	}
}

void ABaseBallGameModeBase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();

	for (const auto& BaseBallPlayerController : AllPlayerControllers)
	{
		ABaseBallPlayerState* BaseBallPS = BaseBallPlayerController->GetPlayerState<ABaseBallPlayerState>();
		if (IsValid(BaseBallPS) == true)
		{
			BaseBallPS->CurrentGuessCount = 0;
		}
	}
}

void ABaseBallGameModeBase::JudgeGame(ABaseBallPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (3 == InStrikeCount)
	{
		ABaseBallPlayerState* BaseBallPS = InChattingPlayerController->GetPlayerState<ABaseBallPlayerState>();
		for (const auto& BaseBallPlayerController : AllPlayerControllers)
		{
			if (IsValid(BaseBallPS) == true)
			{
				FString CombinedMessageString = BaseBallPS->PlayerNameString + TEXT(" has won the game.");
				BaseBallPlayerController->NotificationText = FText::FromString(CombinedMessageString);

				ResetGame();
			}
		}
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& BaseBallPlayerController : AllPlayerControllers)
		{
			ABaseBallPlayerState* BaseBallPS = BaseBallPlayerController->GetPlayerState<ABaseBallPlayerState>();
			if (IsValid(BaseBallPS) == true)
			{
				if (BaseBallPS->CurrentGuessCount < BaseBallPS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}

		if (true == bIsDraw)
		{
			for (const auto& BaseBallPlayerController : AllPlayerControllers)
			{
				BaseBallPlayerController->NotificationText = FText::FromString(TEXT("Draw..."));

				ResetGame();
			}
		}
	}
}
