# Assignment 1: Yonsei Shell (ysh) Implementation

## 1. 문제 정의 (Problem Definition)
본 프로젝트의 목표는 Linux 환경에서 동작하는 사용자 레벨 쉘(Shell) 프로그램 `ysh`를 구현하는 것이다. 커널의 시스템 콜(`fork`, `execvp`, `pipe`, `dup2` 등)을 직접 제어하여 프로세스 생성과 프로세스 간 통신(IPC) 메커니즘을 이해하고, 쉘의 핵심 기능인 명령어 파싱, 파이프라인 처리, 입출력 리다이렉션, 그리고 포그라운드 프로세스 관리를 수행해야 한다.

## 2. 구현 전략 (Implementation Strategy)
**1) 계층적 명령어 파싱 (Hierarchical Parsing)**
명령어 문자열을 단일 패스로 처리하지 않고, 연산자 우선순위에 따라 단계적으로 분해하는 전략을 수립하였다.
* **Level 1 (Sequential):** 세미콜론(`;`)을 기준으로 명령어를 분리하여 순차적으로 실행한다.
* **Level 2 (Pipeline):** 각 순차 블록 내에서 파이프(`|`)를 기준으로 프로세스 그룹을 분리한다.
* **Level 3 (Arguments):** 공백을 기준으로 실행 파일과 인자(Argument)를 파싱한다. `strsep` 함수를 사용하여 문자열을 안전하게 토큰화하였다.

**2) 동시성 파이프라인 처리 (Concurrent Pipeline Execution)**
다중 파이프(`cmd1 | cmd2 | cmd3`) 구현 시, 순차적으로 자식 프로세스를 생성하고 `wait`하는 방식은 파이프 버퍼가 가득 찰 경우 교착 상태(Deadlock)를 유발할 수 있다. 이를 방지하기 위해 **모든 자식 프로세스를 먼저 `fork`하여 동시에 실행**시킨 후, 부모 프로세스가 마지막에 일괄적으로 `wait`하는 구조로 설계하여 데이터 흐름의 병목을 제거하였다.

**3) 리소스 관리 (Resource Management)**
* **File Descriptor:** 파이프 생성 시 사용된 fd가 자식 프로세스 및 부모 프로세스에서 누수되지 않도록 `dup2` 이후 명시적으로 `close`를 수행하였다.
* **Zombie Process:** 부모 프로세스가 `wait`를 통해 자식의 종료 상태를 회수하도록 하여 좀비 프로세스 생성을 방지하였다.

## 3. 결과 (Results)
`ls -al`, `grep`, `cd` 등 기본 명령어와 다중 파이프, 순차 실행 조합이 정상적으로 동작함을 확인하였다.

![Execution Result](images/image-hw1-01.png)

## 4. 기술적 성과 (Technical Accomplishment)
* **Deadlock-Free Architecture:** 파이프라인의 동시 실행 보장을 통해 대용량 데이터 처리 시 발생할 수 있는 행(Hang) 현상을 원천 차단하였다.
* **Robust Parsing:** `strtok` 대신 `strsep`을 활용하여 빈 토큰 처리에 대한 안정성을 확보하였다.
* **Build Automation:** `Makefile`을 도입하여 빌드 과정을 자동화하고 프로젝트의 유지보수성을 높였다.
* **Modern C Style:** 레거시 코드 스타일을 지양하고, 명확한 변수명과 모듈화된 함수(`parse_args`, `execute_piped_commands` 등)를 통해 코드의 가독성과 재사용성을 극대화하였다.