# 운영체제 커널 (xv6-RISC-V) 내부 분석 및 구현 연구

이 저장소는 MIT 6.S081 운영체제 교육용 커널인 **xv6 (RISC-V 아키텍처)**를 기반으로, 프로세스 관리, 메모리 관리, 스케줄링 등 OS의 핵심 메커니즘을 심층 분석하고 확장한 프로젝트 모음입니다.

본 프로젝트는 **커널 레벨(C 언어)**에서 직접 시스템의 기본 동작 원리를 구현함으로써, 저수준 시스템에 대한 깊은 통찰력과 제어 능력을 증명합니다.

## 1. 프로젝트 목표 (Project Goals)

* **시스템 콜 인터페이스:** User Space와 Kernel Space 간의 안전한 데이터 통신 및 프로세스 정보 노출 시스템 구축.
* **커널 스케줄러 설계:** 단순 Round-Robin을 탈피한 Multi-Level Queue (MLQ) 기반의 우선순위 스케줄링 알고리즘 직접 구현.
* **가상 메모리 분석:** RISC-V Sv39 Paging 구조를 탐색하는 도구를 구현하여 VA-PA 변환 과정 및 메모리 사용 현황 시각화.

---

## 2. 프로젝트 개요 (Labs Overview)

총 4개의 핵심 과제를 통해 **프로세스 제어부터 메모리 관리까지**의 영역을 모두 커버했습니다.

| Project | 주제 (Topic) | 핵심 기술 (Key Tech) | 성과 (Achievement Focus) | 상세 보기 |
| :---: | :--- | :--- | :--- | :---: |
| **HW1** | **User Shell** | Fork/Exec, Piping, File Descriptor Management | 복합 쉘 문법 해석 및 유저 레벨 프로세스 흐름 제어 | [Detail View](HW1/README.md) |
| **HW2** | **System Call** | Trap Handling, Syscall Registration, `copyout` | 커널 정보 안전 노출 및 **User-Kernel 데이터 통신** 원리 구현 | [Detail View](HW2/README.md) |
| **HW3** | **Process Scheduling** | Multi-level Queue (MLQ), Linked List, Priority Demotion | **데드락 방지형 스케줄러** 설계 및 Q2/Q1/Q0 상태 천이 로직 구현 | [Detail View](HW3/README.md) |
| **HW4** | **Paging** | Sv39 Page Table Walk, Bitwise Operations, Recursive Traversal | 가상 주소(VA)에서 물리 주소(PA)로의 변환 경로 증명 및 사용 페이지 수 최적화 계산 | [Detail View](HW4/README.md) |

---

## 3. 개발 환경 (Environments & Tools)

* **타겟 OS:** xv6-RISC-V Kernel
* **개발 환경:** Linux (Ubuntu 24.04 LTS via WSL2)
* **에뮬레이터:** QEMU (RISC-V 64-bit)
* **언어:** C, Shell Script
* **도구:** GCC (riscv64-unknown-elf-gcc Cross-Compiler), Git

---

## 4. 결론

본 프로젝트는 저수준 시스템의 동작 원리에 대한 깊은 이해를 제공하며, 시스템 엔지니어링 역량을 증명하는 핵심 포트폴리오입니다.