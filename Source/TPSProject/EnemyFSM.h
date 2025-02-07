#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TPSPlayer.h"
#include "Enemy.h"
#include "EnemyFSM.generated.h"

// 사용할 상태 정의
UENUM(BlueprintType)
enum class EEnemyState : uint8 
{
	Idle, 
	Move,
	Attack,
	Damage,
	Die,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPSPROJECT_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEnemyFSM();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// 상태 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=FSM)
	EEnemyState mState = EEnemyState::Idle;
	
	// 대기 상태
	void IdleState();

	// 이동 상태
	void MoveState();

	// 공격 상태
	void AttackState();

	// 피격 상태
	void DamageState();

	// 죽음 상태
	void DieState();

	// 대기시간
	UPROPERTY(EditDefaultsOnly, Category=FSM)
	float IdleDelayTime = 2.f;

	// 경과시간
	float currentTime = 0.f;

	// 타겟
	UPROPERTY(VisibleAnywhere, Category=FSM)
	class ATPSPlayer* target;
	
	// 소유 액터 
	UPROPERTY()
	class AEnemy* me;

	// 공격 범위
	UPROPERTY(EditAnywhere, Category=FSM)
	float attackRange = 150.f;
	
	// 공격 대기시간
	UPROPERTY(EditAnywhere, Category=FSM)
	float attackDelayTime = 2.f;

	// 피격 알림 이벤트 함수
	void OnDamageProcess();

	// 체력
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=FSM)
	int32 hp = 3;

	// 피격 대기시간
	UPROPERTY(EditAnywhere, Category=FSM)
	float damageDelayTime = 2.f;

	// 아래로 사라지는 속도
	UPROPERTY(EditAnywhere, Category=FSM)
	float dieSpeed = 50.f;


};
