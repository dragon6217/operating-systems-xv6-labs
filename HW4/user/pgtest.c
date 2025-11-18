#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define N 1000
int array[N];

int main(int argc, char *argv[]) {
    // Test 1: Array Address
    printf("&array[N-1]:\n");
    uint64 va = (uint64)&array[N-1];
    printf("Virtual address = 0x%ld\n", va);
    printf("Physical address = 0x%p\n", phyaddr((void*)va));
    printf("PT index at level 2: %d\n", ptidx((void*)va, 2));
    printf("PT index at level 1: %d\n", ptidx((void*)va, 1));
    printf("PT index at level 0: %d\n", ptidx((void*)va, 0));

    // Test 2: Code Address (main)
    printf("\n&main:\n");
    va = (uint64)main;
    printf("Virtual address = 0x%ld\n", va);
    printf("Physical address = 0x%p\n", phyaddr((void*)va));
    printf("PT index at level 2: %d\n", ptidx((void*)va, 2));
    printf("PT index at level 1: %d\n", ptidx((void*)va, 1));
    printf("PT index at level 0: %d\n", ptidx((void*)va, 0));

    // Test 3: Page Count
    printf("\nTotal number of pages = %d\n", pgcnt());

    exit(0);
}