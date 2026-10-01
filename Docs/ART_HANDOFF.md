# ORGEL 아트 전달 작업공간

작업 브랜치: `art/khjhin7962`. 팀 프로젝트의 `EngineAssociation`은 `5.8`이며, 로컬 준비 환경은 UE 5.8.3입니다. 팀 에셋의 기존 저장 버전 5.8.2와 개발자의 현재 설치 버전은 구분합니다. 기존 Source·Config·Content 에셋은 이 준비 작업에서 변경하지 않습니다.

## 에셋 반입 위치

| 리소스 | 팀 프로젝트 Content 아래 경로 |
| --- | --- |
| 플레이어 | `_ORGEL/Art/Characters/CH_Player` |
| 축음기 몬스터 | `_ORGEL/Art/Monsters/MN_Gramophone` |
| 아코디언 보스 | `_ORGEL/Art/Monsters/BS_Accordion` |
| 태엽권총 | `_ORGEL/Art/Weapons/WP_WindupPistol` |
| 브라스 호른 캐논 | `_ORGEL/Art/Weapons/WP_BrassHornCannon` |
| 튜닝포크 캐논 | `_ORGEL/Art/Weapons/WP_TuningForkCannon` |
| 오르골 실린더 발사기 | `_ORGEL/Art/Weapons/WP_MusicBoxLauncher` |

이 경로는 기존 게임 에셋과 충돌하지 않도록 만든 새 아트 작업 구역입니다. 팀에서 정한 경로가 있으면 반입 전에 조정합니다. 현재는 폴더만 준비했으며 모델·리깅·애니메이션을 반입하거나 게임 Blueprint에 연결하지 않았습니다. `.gitkeep`은 빈 폴더를 Git에 보존하는 표식입니다.

## 엔진과 Git

- UE 5.8 프로젝트를 사용합니다. C++ 에디터 대상은 `ORGELEditor Win64 Development`입니다.
- 팀 검토 맵은 `/Game/_ORGEL/Map/LV_Main`입니다. 공용 시작 맵·게임 설정은 변경하지 않습니다.
- 새 `Content/_ORGEL/Art` 폴더 아래의 `.uasset`·`.umap`에만 Git LFS를 적용했습니다. 저장소를 받을 때 Git LFS를 설치하고 `git lfs pull`을 실행합니다.
- 기존 게임 에셋은 LFS로 재변환하지 않았으며 Git 히스토리도 변경하지 않았습니다.
- 빌드 결과·캐시·개인 설정은 기존 `.gitignore`로 제외합니다. Blender·FBX 원본은 개인 아트 작업공간에서 관리하며, 팀 합의 없이 이 Content 폴더에 복사하지 않습니다.

## 기본 셋업 검증

기존 C++ 에디터 빌드는 UE 5.8.3에서 성공했습니다. 2026-10-02에는 native MCP의 toolset 19개와 기존 에셋 16개 읽기, 현재 맵 `/Game/_ORGEL/Map/LV_Main`, 콘텐츠 브라우저의 Art 폴더 이동을 확인했습니다. PIE 시작 후 실행 상태 `true`, 종료 후 `false`와 맵의 `dirty=false`를 확인했습니다. 실제 아트 반입·게임플레이 품질 검수는 후속 작업입니다.

## 전달 순서

1. 이 브랜치에서 모델·텍스처 또는 리깅·모션을 UE로 반입합니다.
2. 크기·재질·변형·장착·재생과 참조 에셋을 확인하고 저장합니다.
3. 에디터를 닫은 뒤 `git status`로 변경 파일을 검토합니다.
4. 해당 에셋과 의존 파일만 명시적으로 add·commit합니다.
5. 전송할 파일 목록·변경 요약·검증 결과·대상 저장소와 브랜치·정확한 commit을 사용자에게 먼저 제시합니다.
6. 해당 전송에 대해 매회 사용자의 명시적인 확인을 받습니다. 확인 전에는 push하지 않습니다.
7. 확인받은 파일·commit·대상 범위만 push합니다. 범위가 바뀌면 다시 제시하고 다시 확인받습니다. 강제 push는 사용하지 않습니다.

Write 권한 확보·셋업 진행·제작 완료·일정 마감 도래는 업로드 승인이 아닙니다. 이전 전송에 대한 확인을 다음 push의 승인으로 재사용하지 않습니다. 팀 GitHub 권한은 로컬 엔진 설정과 별개이며, push 권한이 없으면 저장소 관리자가 계정을 collaborator로 추가해야 합니다. 이 준비 작업은 실제 리소스 업로드 완료를 의미하지 않습니다.
