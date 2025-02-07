#include "EnemyFSM.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy.h"
#include "TPSProject.h"
#include "Components/CapsuleComponent.h"

UEnemyFSM::UEnemyFSM()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	// World에서 ATPSPlayer 찾아왹
	auto actor = UGameplayStatics::GetActorOfClass( GetWorld() , ATPSPlayer::StaticClass() );
	
	// 원래는 !!actor 널체크 해줘야됨
	// ATPSPlayer 타입으로 캐스팅
	target = Cast<ATPSPlayer>(actor);

	// 소유 객체 가져오기
	me = Cast<AEnemy>( GetOwner() );
}


void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 실행창에 상태 로그 출력하기
	FString logMsg = UEnum::GetValueAsString( mState );
	GEngine->AddOnScreenDebugMessage( 0 , 1 , FColor::Red , logMsg );

	switch (mState)
	{
		case EEnemyState::Idle: { IdleState(); } break;
		case EEnemyState::Move: { MoveState(); } break;
		case EEnemyState::Attack: { AttackState(); } break;
		case EEnemyState::Damage: { DamageState(); } break;
		case EEnemyState::Die: { DieState(); }break;
	}
}

void UEnemyFSM::IdleState()
{
	// 시간이 흐르다가
	currentTime += GetWorld()->DeltaTimeSeconds;

	// 만약 경과시간 > 대기시간 되면
	if (currentTime > IdleDelayTime) {
		// 이동 상태로 전환
		mState = EEnemyState::Move;
		
		// 경과시간 초기화 잊지말자!
		currentTime = 0.f;
	}
}

void UEnemyFSM::MoveState()
{
	// 타겟 목적지가 필요
	FVector destination = target->GetActorLocation();

	// 방향이 필요
	FVector dir = destination - me->GetActorLocation();

	// dir 방향으로 이동
	me->AddMovementInput( dir.GetSafeNormal() );

	// 타겟과 거리를 체크해서 attackRange 안으로 들어오면 공격상태로 전환
	// 거리체크 
	if (dir.Size() < attackRange) {
		// 공격 상태로 전환
		mState = EEnemyState::Attack;
	}
}

void UEnemyFSM::AttackState()
{
	// 일정시간에 한번씩 공격
	// 시간이 흐르다가
	currentTime += GetWorld()->DeltaTimeSeconds;

	// 공격시간이 되면
	if (currentTime > attackDelayTime) {
		// 공격로그출력
		PRINT_LOG( TEXT( "Attack" ) );

		// 경과시간 초기화
		currentTime = 0.f;
	}

	// 타겟이 공격범위를 벗어나면 이동상태로 전환
	// 타겟과의 거리가 필요하다
	float distance = FVector::Distance( target->GetActorLocation() , me->GetActorLocation() );

	// 타겟과의 거리가 attackRange를 벗어나면
	if (distance > attackRange) {
		// mState를 Move로
		mState = EEnemyState::Move;
	}

}

void UEnemyFSM::DamageState()
{
	// 시간이 흐르다가
	currentTime += GetWorld()->DeltaTimeSeconds;

	// 경과시간이 대기시간을 초과하면
	if (currentTime > damageDelayTime) {
		// 대기상태로 전환
		mState = EEnemyState::Idle;

		// 경과시간 초기화
		currentTime = 0.f; 
	}
}

void UEnemyFSM::DieState()
{
	// 계속 아래로 내려가고 싶다
	FVector p0 = me->GetActorLocation();
	FVector vt = FVector::DownVector * dieSpeed * GetWorld()->DeltaTimeSeconds;
	FVector P = p0 + vt;
	me->SetActorLocation( P );

	// 만약 2미터 이상 내려왔다면 제거시킨다
	if (P.Z < -200.f) me->Destroy();
}

void UEnemyFSM::OnDamageProcess()
{
	// 체력감소
	--hp;

	// 체력이 남아있는지 체크
	if (hp > 0) {
		// 상태를 피격으로 전환
		mState = EEnemyState::Damage;
	}
	else {
		mState = EEnemyState::Die;

		// Capsule Collision Off
		me->GetCapsuleComponent()->SetCollisionEnabled( ECollisionEnabled::NoCollision );
	}
}