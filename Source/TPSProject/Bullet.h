#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class TPSPROJECT_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	ABullet();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	// 발사체의 이동을 담당할 컴포넌트
	UPROPERTY(VisibleAnywhere, Category=Movement)
	class UProjectileMovementComponent* MovementComp;
	
	// 충돌 컴포넌트
	UPROPERTY(VisibleAnywhere, Category=Collision)
	class USphereComponent* CollisionComp;

	// 외관 컴포넌트
	UPROPERTY(VisibleAnywhere, Category=BodyMesh)
	class UStaticMeshComponent* MeshComp;

};
