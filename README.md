# ⚔️ GuitarSouls

**Unreal Engine 5.6 · C++ · Gameplay Ability System 기반 소울라이크 액션 RPG**

다크 소울 3 스타일의 싱글플레이 소울라이크 액션 RPG로, 컴포넌트 기반 설계에서 **GAS 기반 모듈형 설계**로의 아키텍처 개선을 실증하는 캡스톤 졸업 프로젝트입니다.

> **Branch:** `VerticalSlice` — 재의 묘소 → 군다 구간 버티컬 슬라이스

<br>

## 💡 프로젝트 소개

### 📍 무엇을 만들었나요?

다크 소울 3의 튜토리얼 구간(재의 묘소 → 군다)을 목표로 한 싱글플레이 소울라이크 액션 RPG의 버티컬 슬라이스입니다. 근접 전투, 락온, 회피, 스태미너 관리, 적·보스 AI, 페이즈 전환까지 하나의 플레이 가능한 세그먼트로 완성했습니다.

<br>

### 📍 왜 GAS인가요?

기존 컴포넌트 기반 전투 설계는 캐릭터·상태가 늘수록 클래스 간 상호 참조가 O(N²)로 증가하고 클래스가 비대해집니다. 본 프로젝트는 Unreal의 Gameplay Ability System을 도입해 이 문제를 구조적으로 해소하고, 그 개선 효과를 실제 코드로 실증하는 것을 핵심 논지로 삼습니다.

<br>

### 📍 무엇을 개선했나요?

- **데이터 · 로직 · 표현의 분리** — Attribute(데이터), Gameplay Ability(로직), Montage/Widget(표현)을 각 계층으로 분리
- **태그 기반 상태 관리** — 계층적 Gameplay Tag로 상태를 표현, 열거형·불리언 플래그를 대체
- **데이터 주도 콘텐츠 설계** — 무기·데미지·공격 패턴을 DataAsset과 SetByCaller GE로 구성

<br>

### 📍 프로젝트 목표

**레거시를 답습하지 않고, 참고로만 삼아 재설계한다 🔥**

- 레거시 `GuitarSouls` 모듈은 참조 대상으로만 유지하고, 모든 신규 로직은 `GuitarSoulsGAS` 모듈에 작성
- 기획 변경이 잦은 값은 Blueprint로, 런타임 로직은 C++로 경계를 명확히 분리
- 확장성 우선 — '지금 단순'보다 '나중에 바꾸기 쉬운' 구조를 선택

<br>

---

## 🏛️ 아키텍처

### 📍 모듈 구조

```
Source/
├── GuitarSouls/         레거시 (컴포넌트 기반, 제거 예정)
└── GuitarSoulsGAS/      GAS 전담 모듈 (GuitarSouls PublicDependency)
    ├── Character/       캐릭터 계층 (Base / Player / Enemy / Boss)
    ├── GA/              Gameplay Abilities
    ├── Attribute/       AttributeSet
    ├── Component/       WeaponCollisionComponent
    ├── Item/            Weapon / WeaponData
    ├── AI/              AIController / BTTaskNode / BTService / BTDecorator
    ├── Tags/            Native Gameplay Tags
    └── UI/              GSGASUserWidget 계열
```

<br>

### 📍 캐릭터 계층

```
ACharacter
└── AGSGASCharacterBase          ASC 캐시, EquippedWeapon, StartAbilities
    ├── AGSGASCharacterPlayer    ASC 소유: PlayerState
    ├── AGSGASCharacterEnemy     ASC 직접 보유, 순찰 / 경보
    └── AGSGASCharacterBoss      Enemy 상속, 페이즈 전환
```

| 캐릭터 | ASC 소유 | 초기화 시점 |
| --- | --- | --- |
| 플레이어 | `AGSGASPlayerState` | `PossessedBy` |
| 적 / 보스 | Character 직접 보유 | `PossessedBy` |

<br>

---

## 🎮 핵심 시스템

### 📍 전투

- **약공 / 강공 콤보** — 태그당 몽타주 1개, 섹션 분기 방식. `Character.State.ComboWindow` 태그 구간에서 다음 입력을 수락한 뒤 `MontageJumpToSection`으로 연결
- **무기 판정** — `AnimNotifyState`는 ASC 태그 add/remove만 수행하고, GA가 태그 이벤트를 감지해 `WeaponCollisionComponent`의 소켓 기반 SphereTrace를 On/Off
- **데미지 파이프라인** — `GE_GSDamage`에 `Data.Damage`를 SetByCaller로 주입 → `PostGameplayEffectExecute`에서 Health 차감 → 생존 시 `Character.Action.HitReaction` 이벤트 발송
- **4방향 히트리액션** — 공격자–피격자 상대 Yaw로 Front / Back / Left / Right 섹션 결정

<br>

### 📍 적 / 보스 AI

- **행동 선택** — Blackboard 플래그 대신 Gameplay Tag(`AI.Behavior.*`)와 GAS 어트리뷰트 기반 BTService · BTDecorator로 판단
- **공격 실행** — `BTTaskNode_GASPerformAttack`이 `SendGameplayEventToActor`로 GA를 발동(`TriggerEventData` 전달). 보스는 Shuffle Bag 방식 패턴 태스크로 모든 패턴 1회를 보장하며 랜덤 공격
- **페이즈 전환** — HP 임계값 도달 시 `AttributeSet`이 loose 태그를 부여, `GA_BossPhaseTransition`이 1회 발동. 전환 중 `Character.State.Immune`으로 데미지 면역

<br>

### 📍 스탯 / UI

- `GSGASUserWidget::SetASC()` 호출 시 `InitializeWidget()`이 트리거되어 어트리뷰트 변경 델리게이트를 바인딩
- 플레이어 HUD(HP · 스태미너 · 포션 · 상호작용 프롬프트), 보스 HP 바

<br>

---

## 📚 기술 스택

- <b>Engine</b>

  <img src="https://img.shields.io/badge/Unreal_Engine_5.6-0E1128?style=for-the-badge&logo=unrealengine&logoColor=white">
  <img src="https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=cplusplus&logoColor=white">

- <b>Framework</b>

  <img src="https://img.shields.io/badge/Gameplay_Ability_System-313131?style=for-the-badge&logoColor=white">
  <img src="https://img.shields.io/badge/Gameplay_Tags-313131?style=for-the-badge&logoColor=white">
  <img src="https://img.shields.io/badge/Behavior_Tree-2088FF?style=for-the-badge&logoColor=white">
  <img src="https://img.shields.io/badge/Enhanced_Input-FF6C37?style=for-the-badge&logoColor=white">
  <img src="https://img.shields.io/badge/UMG-6DB33F?style=for-the-badge&logoColor=white">

- <b>Tools</b>

  <img src="https://img.shields.io/badge/github-181717?style=for-the-badge&logo=github&logoColor=white">
  <img src="https://img.shields.io/badge/notion-000000?style=for-the-badge&logo=notion&logoColor=white">

<br>

---

## ⚙️ 설계 원칙

| 원칙 | 내용 |
| --- | --- |
| 레거시 참조 통제 | 레거시 심볼 직접 참조 금지, GAS 태그는 별도 정의 |
| BP vs C++ 경계 | GE 수치·GA 활성화/차단 태그는 Blueprint 디폴트, 런타임 로직만 C++ |
| 로그 통일 | 전 모듈 `GSGAS_LOG` 매크로 사용 |

<br>

---

## 🗺️ 로드맵

| 구분 | 항목 |
| --- | --- |
| 진행 중 | 적 AI 연동 마무리, 보스 행동 선택 서비스 고도화 |
| 예정 | `GA_Block` / `GA_Parry` + 입력 바인딩 |
| 2학기 | 시네마틱, 최적화, QX 기능(감도 설정 등) |
