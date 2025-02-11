#pragma once

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

	// 플레이어 이동 속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=TPSPlayerAnimInstance)
	float Speed = 0.f;

};