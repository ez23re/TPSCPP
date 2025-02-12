#include "TPSPlayerAnimInstance.h"
#include "TPSPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

void UTPSPlayerAnimInstance::NativeUpdateAnimation( float DeltaSeconds )
{
	Super::NativeUpdateAnimation( DeltaSeconds );

	ATPSPlayer* player = Cast<ATPSPlayer>( TryGetPawnOwner() );
	if (player == nullptr) {
		return;
	}

	FVector velocity = player->GetVelocity();
	FVector forwardvector = player->GetActorForwardVector();

	// 앞뒤 이동속도
	Speed = FVector::DotProduct( velocity , forwardvector );

	// 좌우 이동속도
	FVector rightVector = player->GetActorRightVector();
	Direction = FVector::DotProduct( rightVector , velocity );

	
	// 플레이어가 현재 공중에 있는지 저장
	auto movement = player->GetCharacterMovement();
	if (movement != nullptr)
	{
		isInAir = movement->IsFalling();
	}
	// isInAir = player->GetCharacterMovement()->IsFalling();


}

void UTPSPlayerAnimInstance::PlayAttackAnim()
{
	if (AttackAnimMontage == nullptr) return;
	Montage_Play( AttackAnimMontage );
}