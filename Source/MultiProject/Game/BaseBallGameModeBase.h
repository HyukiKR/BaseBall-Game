// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BaseBallGameModeBase.generated.h"

class ABaseBallPlayerController;

/**
 * 
 */
UCLASS()
class MULTIPROJECT_API ABaseBallGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void OnPostLogin(AController* NewPlayer) override;

	FString GenerateSecretNumber();

	bool IsGuessNumberString(const FString& InNumberString);

	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);

	virtual void BeginPlay() override;

	void PrintChatMessageString(ABaseBallPlayerController* InChattingPlayerController, const FString& InChatMessageString);
	
	void IncreaseGuessCount(ABaseBallPlayerController* InChattingPlayerController);

	void ResetGame();

	void JudgeGame(ABaseBallPlayerController* InChattingPlayerController, int InStrikeCount);

protected:
	FString SecretNumberString;

	TArray<TObjectPtr<ABaseBallPlayerController>> AllPlayerControllers;
};
