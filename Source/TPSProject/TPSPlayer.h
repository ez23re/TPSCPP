#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Bullet.h"
#include "Blueprint/UserWidget.h"
#include "TPSPlayer.generated.h"

UCLASS()
class TPSPROJECT_API ATPSPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	ATPSPlayer();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UPROPERTY( EditDefaultsOnly , Category=Camera)
	class USpringArmComponent* springArmComp;

	// Gun Skeletal Mesh
	UPROPERTY(VisibleAnywhere, Category=GunMesh)
	class USkeletalMeshComponent* GunMeshComp;

	// 총알 공장
	UPROPERTY( EditDefaultsOnly , Category = BulletFactory)
	TSubclassOf<class ABullet> BulletFactory;

	UPROPERTY(EditDefaultsOnly, Category=Input)
	class UInputAction* IA_Fire;

	// 총알 발사 처리함수
	void InputFire(const FInputActionValue& inputValue);

	// 스나이퍼건 스태틱메시 추가
	UPROPERTY(VisibleAnywhere, Category=GunMesh)
	class UStaticMeshComponent* SniperGunMesh;

	// 유탄총 사용중인지 여부
	bool bUsingGrenadeGun = true;

	// 유탄총으로 변경
	void ChangeToGrenadeGun( const FInputActionValue& inputValue );

	// 스나이퍼건으로 변경
	void ChangeToSniperGun( const FInputActionValue& inputValue );

	UPROPERTY( VisibleAnywhere , BlueprintReadOnly , Category = Camera )
	class UCameraComponent* tpsCamComp;

	// 스나이퍼모드
	UPROPERTY(EditDefaultsOnly, Category=Input)
	class UInputAction* IA_Sniper;

	void SniperAim( const FInputActionValue& inputValue );
	bool bSniperAim = false;

	// 스나이퍼 UI 위젯 공장
	UPROPERTY(EditDefaultsOnly, Category=SniperUI)
	TSubclassOf<class UUserWidget> SniperUIFactory; // 헤더추가 해줘야됨?

	// 스나이퍼 UI Widget 인스턴스
	UPROPERTY()
	class UUserWidget* sniperUI;

	// 총알 파편 효과 공장
	UPROPERTY(EditAnywhere, Category=BulletEffect)
	class UParticleSystem* BulletEffectFactory;

	// 일반 조준 크로스헤어UI 위젯
	UPROPERTY(EditDefaultsOnly, Category=SniperUI)
	TSubclassOf<class UUserWidget> CrossHairUIFactory;

	// 크로스헤어 인스턴스
	class UUserWidget* _CrossHairUI;


	// Input
	UPROPERTY(EditDefaultsOnly, Category=Input)
	class UInputMappingContext* IMC_TPS;
	
	UPROPERTY(EditDefaultsOnly, Category=Input)
	class UInputAction* IA_LookUp; // Mouse 상하 회전
	
	UPROPERTY(EditDefaultsOnly, Category=Input)
	class UInputAction* IA_Turn; // Mouse 좌우 회전

	UPROPERTY(EditDefaultsOnly, Category=Input)
	class UInputAction* IA_PlayerMove; // 사용자 좌우 이동

	UPROPERTY(EditDefaultsOnly, Category=Input)
	class UInputAction* IA_GrenadeGun;

	UPROPERTY(EditDefaultsOnly, Category=Input)
	class UInputAction* IA_SniperGun;

	UPROPERTY(EditDefaultsOnly, Category=Input)
	class UInputAction* IA_Jump;
	
	// 이동 속도
	UPROPERTY(EditAnywhere, Category=PlayerSetting)
	float WalkSpeed = 200.f;
	
	// 달리기 속도
	UPROPERTY(EditAnywhere, Category=PlayerSetting)
	float RunSpeed = 600.f;
	
	// 이동 방향
	FVector Direction;


	void Turn(const FInputActionValue& inputValue);
	void LookUp( const FInputActionValue& inputValue );
	void Move( const FInputActionValue& inputValue );
	void InputJump( const FInputActionValue& inputValue );

	// 달리기
	UPROPERTY(EditDefaultsOnly, Category=Input)
	class UInputAction* IA_Run;

	void InputRun();
};
