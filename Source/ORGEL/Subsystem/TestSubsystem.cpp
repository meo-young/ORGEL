#include "TestSubsystem.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameplayEffect.h"
#include "InputCoreTypes.h"
#include "ORGameplayTags.h"

bool UTestSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// Shipping에서는 생성하지 않고 나머지 빌드에서는 부모의 생성 조건을 따릅니다.
	return !UE_BUILD_SHIPPING && Super::ShouldCreateSubsystem(Outer);
}

void UTestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 기존 피해 효과를 사용하여 공통 피격 및 사망 처리를 유지합니다.
	DamageEffectClass = LoadClass<UGameplayEffect>(nullptr, TEXT("/Game/_ORGEL/Blueprint/AbilitySystem/Effect/Damage/GE_Damage_SetByCaller.GE_Damage_SetByCaller_C"));

	// 효과를 불러오지 못한 경우 테스트 입력을 등록하지 않습니다.
	if (!DamageEffectClass)
	{
		UE_LOG(LogTemp, Error, TEXT("테스트 피해 GameplayEffect를 불러오지 못했습니다."));
		return;
	}

	// 플레이어 입력 처리가 끝난 뒤 해당 프레임의 키 입력을 확인합니다.
	FWorldDelegates::OnWorldPostActorTick.AddWeakLambda(this, [this](UWorld* World, ELevelTick, float)
	{
		// 다른 PIE 인스턴스와 시작 전 월드를 제외하고 일시정지 중에는 실행하지 않습니다.
		if (World->GetGameInstance() != GetGameInstance() || !World->HasBegunPlay() || World->IsPaused())
		{
			return;
		}

		// 컨트롤러가 아직 없거나 로컬 플레이어가 아니면 입력 확인을 생략합니다.
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (!PlayerController || !PlayerController->IsLocalController())
		{
			return;
		}

		// 1번 키를 누른 순간에만 테스트 피해 함수를 호출합니다.
		if (PlayerController->WasInputKeyJustPressed(EKeys::One))
		{
			ApplyTestDamage(PlayerController);
		}
	});
}

void UTestSubsystem::Deinitialize()
{
	// 종료된 서브시스템에 월드 콜백이 남지 않도록 해제합니다.
	FWorldDelegates::OnWorldPostActorTick.RemoveAll(this);
	DamageEffectClass = nullptr;

	Super::Deinitialize();
}

void UTestSubsystem::ApplyTestDamage(APlayerController* PlayerController)
{
	// 빙의 전이나 캐릭터 제거 후에는 피해를 적용하지 않습니다.
	APawn* PlayerPawn = PlayerController->GetPawn();
	if (!PlayerPawn)
	{
		return;
	}

	// 현재 플레이어에 연결된 ASC가 준비된 경우에만 처리합니다.
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PlayerPawn);
	if (!ASC)
	{
		return;
	}

	// 기존 Damage 속성 처리 경로로 피해 10을 전달합니다.
	FGameplayEffectSpecHandle DamageSpec = ASC->MakeOutgoingSpec(DamageEffectClass, 1.0f, ASC->MakeEffectContext());
	DamageSpec.Data->SetSetByCallerMagnitude(ORGameplayTags::SetByCaller_Damage, 1.0f);
	ASC->ApplyGameplayEffectSpecToSelf(*DamageSpec.Data.Get());
}