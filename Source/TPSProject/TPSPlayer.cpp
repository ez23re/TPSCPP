#include "TPSPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyFSM.h"

ATPSPlayer::ATPSPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Mesh에 퀸을 로드해서 넣고 싶다
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequin_UE4/Meshes/SK_Mannequin.SK_Mannequin'"));

	// 만약 파일 읽기가 성공했다면
	if (TempMesh.Succeeded()) {
		// 로드한 메시를 넣어주고 싶다
		GetMesh()->SetSkeletalMesh(TempMesh.Object);

		// 위치벡터, 회전벡터 값 넣어주기
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0,-90,0));
	}

	// TPS Camera를 붙이고 싶다
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("springArmComp"));
	springArmComp->SetupAttachment(RootComponent);
	springArmComp->SetRelativeLocation(FVector(0.f, 60.f, 80.f));
	springArmComp->TargetArmLength = 300.f;
	springArmComp->bUsePawnControlRotation = true;

	tpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("tpsCamComp"));
	tpsCamComp->SetupAttachment(springArmComp);
	tpsCamComp->bUsePawnControlRotation = false;

	this->bUseControllerRotationPitch = false;
	this->bUseControllerRotationRoll = false;
	this->bUseControllerRotationYaw = true;

	// Gun Skeletal Mesh Component 등록
	GunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>( TEXT( "GunMeshComp" ) );

	// 부모 컴포넌트를 Mesh 컴포넌트로 설정
	GunMeshComp->SetupAttachment(GetMesh());

	// 스켈레탈 메시 데이터 로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh( TEXT( "/Script/Engine.SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'" ) );

	// 데이터 로드가 성공 했다면
	if( TempGunMesh.Succeeded() ) {
		// 스켈레탈 메시 데이터 할당
		GunMeshComp->SetSkeletalMesh( TempGunMesh.Object );
		
		// 총 위치 설정
		GunMeshComp->SetRelativeLocation( FVector( 0 , 60 , 120 ) );
	}

	// 스나이퍼건 컴포넌트 등록
	SniperGunMesh = CreateDefaultSubobject<UStaticMeshComponent>( TEXT("SniperGunMesh" ));
	SniperGunMesh->SetupAttachment( GetMesh() );
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempSniperMesh ( TEXT( "/Script/Engine.StaticMesh'/Game/SniperGun/sniper11.sniper11'" ) );

	if (TempSniperMesh.Succeeded()) {
		// 성공시 스태틱 메시 데이터 할당
		SniperGunMesh->SetStaticMesh( TempSniperMesh.Object );

		// 총 위치 설정
		SniperGunMesh->SetRelativeLocation( FVector( 0 , 50 , 120 ));

		// 총 크기 설정
		SniperGunMesh->SetRelativeScale3D( FVector( 0.15f ) );

	}
}

void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	auto pc = Cast<APlayerController>( Controller );
	if (pc) {
		auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());

		if (subsystem) {
			subsystem->AddMappingContext( IMC_TPS , 0 );
		}
	}

	// JumpMaxCount = 5;
	
	// 스나이퍼 UI 위젯 인스턴스 생성
	sniperUI = CreateWidget( GetWorld() , SniperUIFactory );

	// 일반 조준 UI 크로스헤어 인스턴스 생성
	_CrossHairUI = CreateWidget( GetWorld() , CrossHairUIFactory );

	// 일반 조준 UI를 보이도록 등록
	_CrossHairUI->AddToViewport();
	
	// 기본으로 스나이퍼 건을 사용하도록 설정
	ChangeToSniperGun( FInputActionValue() );
	
}

void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 절대좌표로 받은 벡터를 캐릭터 상대좌표로 바꾸고 싶다
	Direction = FTransform( GetControlRotation() ).TransformVector( Direction );

	//FVector p0 = GetActorLocation();
	//FVector vt = WalkSpeed * Direction * DeltaTime;
	//FVector p = p0 + vt;
	//SetActorLocation( p );
	AddMovementInput( Direction );
	
	Direction = FVector::ZeroVector; // 안하면 떼도 계속 움직임	
}

void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	auto PlayerInput = Cast<UEnhancedInputComponent>( PlayerInputComponent );
	if (PlayerInput) {

		PlayerInput->BindAction( IA_LookUp , ETriggerEvent::Triggered , this , &ATPSPlayer::LookUp );
		PlayerInput->BindAction( IA_Turn , ETriggerEvent::Triggered , this , &ATPSPlayer::Turn );

		PlayerInput->BindAction( IA_PlayerMove , ETriggerEvent::Triggered , this , &ATPSPlayer::Move );
		PlayerInput->BindAction( IA_Jump , ETriggerEvent::Started , this , &ATPSPlayer::InputJump );
		PlayerInput->BindAction( IA_Fire , ETriggerEvent::Started , this , &ATPSPlayer::InputFire );

		PlayerInput->BindAction( IA_GrenadeGun , ETriggerEvent::Started , this , &ATPSPlayer::ChangeToGrenadeGun );
		PlayerInput->BindAction( IA_SniperGun , ETriggerEvent::Started , this , &ATPSPlayer::ChangeToSniperGun );

		PlayerInput->BindAction( IA_Sniper , ETriggerEvent::Started , this , &ATPSPlayer::SniperAim );
		PlayerInput->BindAction( IA_Sniper , ETriggerEvent::Completed , this , &ATPSPlayer::SniperAim );

	}
}

void ATPSPlayer::InputFire( const FInputActionValue& inputValue )
{
	// 유탄총 사용시
	if (bUsingGrenadeGun == true) {
	FTransform FirePosition = GunMeshComp->GetSocketTransform(TEXT("FirePosition"));
	GetWorld()->SpawnActor<ABullet>( BulletFactory , FirePosition);
	} 
	else { // 스나이퍼 건 사용시
		// LineTrace 의 시작위치
		FVector startPos = tpsCamComp->GetComponentLocation();

		// LineTrace 의 종료위치
		FVector endPos = tpsCamComp->GetComponentLocation() + tpsCamComp->GetForwardVector() * 5000.f;

		// LineTrace 의 충돌정보 담을 변수
		FHitResult hitInfo;

		// 충돌 옵션 설정 변수
		FCollisionQueryParams params;

		// 자기 자신은 총돌에서 제외
		params.AddIgnoredActor( this );

		// Channel filter를 이용한 LineTrace 충돌검출
		bool bHit = GetWorld()->LineTraceSingleByChannel( hitInfo , startPos , endPos, ECollisionChannel::ECC_Visibility, params);

		// LineTrace가 충돌했을 때 
		if (bHit == true) {
			// 충돌처리 -> 충돌효과 표현
			// 총알 파편 효과 트랜스폼 
			FTransform BulletTrans;

			// 부딪힌 위치를 할당
			BulletTrans.SetLocation( hitInfo.ImpactPoint );

			// 총알 파편 효과 인스턴스 생성
			UGameplayStatics::SpawnEmitterAtLocation( GetWorld() , BulletEffectFactory , BulletTrans );

			auto hitComp = hitInfo.GetComponent();

			// 만약에 컴포넌트에 물리가 적용되어 있다면
			if (hitComp&&hitComp->IsSimulatingPhysics()) {
				// 조준한 방향이 필요
				FVector dir = (endPos - startPos).GetSafeNormal();

				// 날려 버릴 힘 ( F = ma )
				FVector force = dir * hitComp->GetMass() * 500000;

				// 그 방향으로 날려버리고 싶다
				hitComp->AddForceAtLocation( force , hitInfo.ImpactPoint );
			}
			// 부딪힌 대상이 적인지 체크
			auto enemy = hitInfo.GetActor()->GetDefaultSubobjectByName( TEXT( "FSM" ) );
			if (enemy) {
				auto enemyFSM = Cast<UEnemyFSM>( enemy );
				enemyFSM->OnDamageProcess();
			}
		}
	}
}

void ATPSPlayer::ChangeToGrenadeGun( const FInputActionValue& inputValue )
{
	// 유탄총 사용중으로 체크
	bUsingGrenadeGun = true;
	SniperGunMesh->SetVisibility( !bUsingGrenadeGun );
	GunMeshComp->SetVisibility( bUsingGrenadeGun );
}

void ATPSPlayer::ChangeToSniperGun( const FInputActionValue& inputValue )
{
	bUsingGrenadeGun = false;
	SniperGunMesh->SetVisibility( true );
	GunMeshComp->SetVisibility( false );
}

void ATPSPlayer::SniperAim( const FInputActionValue& inputValue )
{
	// 스나이퍼건 모드가 아니라면 처리하지 않는다
	if (bUsingGrenadeGun) {
		return;
	}
	
	// Pressed 입력 처리 : Sniper 조준 모드 활성화
	if (bSniperAim == false) {
		bSniperAim = true; 
		sniperUI->AddToViewport();
		tpsCamComp->SetFieldOfView( 45 ); // 카메라 시야각 FOV 45도로
		// 일반 조준 UI를 제거
		_CrossHairUI->RemoveFromParent();
	}
	else {
		bSniperAim = false; // Released 입력 처리 :  Sniper 조준 모드 비활성화
		sniperUI->RemoveFromParent();
		tpsCamComp->SetFieldOfView(90); // 카메라 시야각 복원
		// 일반 조준 UI 등록
		_CrossHairUI->AddToViewport();
	}
}

void ATPSPlayer::Turn( const FInputActionValue& inputValue )
{
	float value = inputValue.Get<float>();
	AddControllerYawInput( value );
}

void ATPSPlayer::LookUp( const FInputActionValue& inputValue )
{
	float value = inputValue.Get<float>();
	AddControllerPitchInput( value );
}

void ATPSPlayer::Move( const FInputActionValue& inputValue )
{
	FVector2D value = inputValue.Get<FVector2D>();

	// 상하 입력처리
	Direction.X = value.X;

	// 좌우 입력처리
	Direction.Y = value.Y;
}

void ATPSPlayer::InputJump( const FInputActionValue& inputValue )
{
	Jump();
}