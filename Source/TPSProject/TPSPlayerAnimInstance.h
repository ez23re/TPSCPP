﻿#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TPSPlayerAnimInstance.generated.h"

UCLASS()
class TPSPROJECT_API UTPSPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	

public:
	// 매 프레임 갱신되는 함수
	virtual void NativeUpdateAnimation( float DeltaSeconds ) override;

	// 플레이어 앞뒤 이동 속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=TPSPlayerAnim)
	float Speed = 0.f;
	
	// 플레이어 좌우 이동 속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=TPSPlayerAnim)
	float Direction = 0.f;

	// 플레이어가 공중에 있는지 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=TPSPlayerAnim)
	bool isInAir = false;

	// 재생할 공격 애니메이션 몽타쥬
	UPROPERTY(EditDefaultsOnly, Category=TPSPlayerAnim)
	class UAnimMontage* AttackAnimMontage;

	// 공격 애니메이션 재생 함수
	void PlayAttackAnim();
};