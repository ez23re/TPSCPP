#include "TPSPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "Components/SkeletalMeshComponent.h"

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

	JumpMaxCount = 5;
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