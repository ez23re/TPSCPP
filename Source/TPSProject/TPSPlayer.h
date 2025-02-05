#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
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

	UPROPERTY( EditDefaultsOnly, Category=Camera)
	class UCameraComponent* tpsCamComp;

	UPROPERTY(EditDefaultsOnly, Category=Input)
	class UInputMappingContext* IMC_TPS;
	
	UPROPERTY(EditDefaultsOnly, Category=Input)
	class UInputAction* IA_LookUp; // Mouse 상하 회전
	
	UPROPERTY(EditDefaultsOnly, Category=Input)
	class UInputAction* IA_Turn; // Mouse 좌우 회전

	UPROPERTY(EditDefaultsOnly, Category=Input)
	class UInputAction* IA_PlayerMove; // 사용자 좌우 이동

	UPROPERTY(EditDefaultsOnly, Category=Input)

	class UInputAction* IA_Jump;
	
	UPROPERTY(EditAnywhere, Category=PlayerSetting)
	float WalkSpeed = 600.f;

	FVector Direction;


	void Turn(const FInputActionValue& inputValue);
	void LookUp( const FInputActionValue& inputValue );
	void Move( const FInputActionValue& inputValue );
	void InputJump( const FInputActionValue& inputValue );
};
