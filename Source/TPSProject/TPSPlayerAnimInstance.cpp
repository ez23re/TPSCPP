#include "TPSPlayerAnimInstance.h"
#include "TPSPlayer.h"

void UTPSPlayerAnimInstance::NativeUpdateAnimation( float DeltaSeconds )
{
	Super::NativeUpdateAnimation( DeltaSeconds );

	ATPSPlayer* player = Cast<ATPSPlayer>( TryGetPawnOwner() );
	if (player == nullptr) {
		return;
	}

	FVector velocity = player->GetVelocity();
	FVector forwardvector = player->GetActorForwardVector();
	Speed = FVector::DotProduct( velocity , forwardvector );
}