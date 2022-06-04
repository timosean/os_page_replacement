#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int* generate_ref_arr(size_t sz, size_t page_max);
int lru(int* ref_arr, size_t ref_arr_sz, size_t frame_sz);

// Driver code
//  Usage: ./program {ref_arr_size} {page_max} {frame_size}
int main(int argc, char** argv) {
    if (argc < 4) return -1;
    
    int ref_arr_sz = atoi(argv[1]); //Reference String의 길이
    int page_max = atoi(argv[2]);   //최대 페이지 번호
    int frame_sz = atoi(argv[3]);   //프레임 사이즈

    int* ref = generate_ref_arr(ref_arr_sz, page_max);
    int page_faults = lru(ref, ref_arr_sz, frame_sz);
    printf("%d\n", page_faults);
    free(ref);

    return 0;
}

int* generate_ref_arr(size_t sz, size_t page_max) {
    int* ref_arr = (int*) malloc(sizeof(int) * sz);

    srand((unsigned int)time(NULL));
    
    // TODO :: Generate Random Refernece String
    for(int i=0; i < sz; i++) {
        //0부터 page_max 사이의 숫자를 랜덤으로 뽑아서 Reference String에 넣는다.
        ref_arr[i] = rand() % (page_max + 1);
    }
    return ref_arr;
}

//Clock Algorithm을 이용한 LRU Approximation
int lru(int* ref_arr, size_t ref_arr_sz, size_t frame_sz) {
    int i, j;
    int page_faults = 0;
    
    // Initializing frames (프레임 사이즈만큼 프레임들을 생성하고 모두 EMPTY_FRAME으로 초기화시킨다.)
    int* frames = (int*) malloc(sizeof(int) * frame_sz);
    for (i=0; i<frame_sz; i++) frames[i] = -1;

    // TODO

    return page_faults;
}

