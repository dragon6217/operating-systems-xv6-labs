# Assignment 2: System Call Implementation

## 1. 문제 정의 (Problem Definition)
본 프로젝트의 목표는 교육용 운영체제인 xv6-riscv 커널에 새로운 시스템 콜 `getppid`와 `getpname`을 추가하여 운영체제의 기능을 확장하는 것이다. 유저 레벨의 프로세스가 커널 내부의 Process Control Block (PCB) 정보에 접근할 수 있도록 시스템 콜 인터페이스를 등록하고, 커널 공간(Kernel Space)의 데이터를 유저 공간(User Space)으로 안전하게 전달하는 메모리 메커니즘을 구현해야 한다.

## 2. 구현 전략 (Implementation Strategy)

**1) System Call Registration Chain**
시스템 콜이 호출되어 처리되기까지의 전체 경로를 다음 단계에 따라 연결하였다.
* **User Interface:** `user/user.h`에 함수 프로토타입을 선언하고, `user/usys.pl` 스크립트에 진입점(Entry Point)을 등록하여 RISC-V `ecall` 명령어를 포함한 어셈블리 코드가 자동 생성되도록 구성하였다.
* **Kernel Identifier:** `kernel/syscall.h`에 고유 번호(`SYS_getppid`, `SYS_getpname`)를 할당하였다.
* **Dispatcher Mapping:** `kernel/syscall.c`의 함수 포인터 배열에 핸들러 함수를 매핑하여, 트랩 발생 시 적절한 커널 함수가 호출되도록 설계하였다.

**2) 부모 프로세스 ID 조회 (getppid)**
* 현재 프로세스의 메타데이터를 담고 있는 `struct proc` 구조체에 접근하여 구현하였다.
* `myproc()->parent` 포인터가 유효한지(Not Null) 검증한 후, 존재할 경우 부모의 PID를 반환하고, `init` 프로세스와 같이 부모가 없는 경우 0을 반환하여 커널 패닉을 방지하였다.

**3) 프로세스 이름 복사 (getpname)**
* **Argument Handling:** 유저가 전달한 버퍼의 포인터를 가져오기 위해 `argaddr(0, &addr)`을 사용하여 레지스터 값을 주소로 변환하였다.
* **Safe Memory Copy (Copyout):** 커널 메모리(`p->name`)를 유저 가상 메모리 주소(`addr`)로 직접 복사하는 것은 보안 및 페이징 위반을 초래할 수 있다. 따라서 페이지 테이블을 참조하여 가상 주소를 물리 주소로 변환하고 안전하게 데이터를 복사하는 `copyout()` 함수를 사용하여 이를 해결하였다.

## 3. 결과 (Results)
테스트 프로그램(`syscalltest`)을 통해 구현된 시스템 콜을 호출한 결과, 현재 프로세스의 PID, 부모 프로세스의 PID(PPID), 그리고 프로세스 이름이 정상적으로 반환됨을 확인하였다.

![Execution Result](images/image-hw2-01.png)

## 4. 기술적 성과 (Technical Accomplishment)
* **Kernel Internals Understanding:** 시스템 콜이 하드웨어 인터럽트(Trap)를 거쳐 커널 함수로 연결되는 전체 흐름을 파악하고 코드로 구현하였다.
* **Cross-Domain Memory Access:** 서로 다른 보호 도메인(Kernel vs User) 간에 데이터를 전송할 때 필수적인 `copyout` 매커니즘을 적용하여, 운영체제의 메모리 보호 기법을 준수하는 안전한 코드를 작성하였다.