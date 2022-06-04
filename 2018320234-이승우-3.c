#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#define EMPTY_FRAME -1

int* generate_ref_arr(size_t sz, size_t page_max);
int lru(int* ref_arr, size_t ref_arr_sz, size_t frame_sz, size_t page_max);

int _contains(int* arr, size_t sz, int target) {
    int i = 0;
    for (i=0; i<sz; i++) {
        if (arr[i] == target) return i;
    }
    return -1;
}

int _max(int* arr, size_t sz) {
    int i;
    int max = INT_MIN;

    for (i=0; i<sz; i++) {
        if (arr[i] > max) max = arr[i];
    }

    return max;
}

int _min(int* arr, size_t sz) {
    int i;
    int min = INT_MAX;

    for (i=0; i<sz; i++) {
        if (arr[i] < min) min = arr[i];
    }

    return min;
}

// Driver code
//  Usage: ./program {ref_arr_size} {page_max} {frame_size}
int main(int argc, char** argv) {
    if (argc < 4) return -1;
    
    int ref_arr_sz = atoi(argv[1]); //Reference String의 길이
    int page_max = atoi(argv[2]);   //최대 페이지 번호
    int frame_sz = atoi(argv[3]);   //프레임 사이즈

    int* ref = generate_ref_arr(ref_arr_sz, page_max);
    int testRef[] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1};

    //int page_faults = lru(testRef, ref_arr_sz, frame_sz, page_max);
    int page_faults = lru(ref, ref_arr_sz, frame_sz, page_max);
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

//Additional Reference Bits Algorithm의 구현
int lru(int* ref_arr, size_t ref_arr_sz, size_t frame_sz, size_t page_max) {
    int i, j, is_fault, target;
    int page_faults = 0;

    //각 페이지 별 Additional Bit의 정보를 담을 배열 및 초기화
    int* additional = (int*) malloc(sizeof(int) * (page_max + 1));
    for(i=0; i<page_max+1; i++) additional[i] = 0;
    
    // Initializing frames (프레임 사이즈만큼 프레임들을 생성하고 모두 EMPTY_FRAME으로 초기화시킨다.)
    int* frames = (int*) malloc(sizeof(int) * frame_sz);
    for (i=0; i<frame_sz; i++) frames[i] = -1;

    // TODO
    // 페이지가 참조될 때마다 해당 페이지의 부가비트에 +128 해준다.

    // Iterating reference string
    for (i=0; i<ref_arr_sz; i++) {
        is_fault = _contains(frames, frame_sz, ref_arr[i]);

        //Code for debugging
        //printf("[현재 additional bits 상황]\n");
        //for(int e=0; e<page_max+1; e++) {
        //    printf("%d ", additional[e]);
        //}
        //printf("\n");

        // Miss (page fault occurred)
        if (is_fault == -1) {
            int empty_idx = _contains(frames, frame_sz, EMPTY_FRAME);

            // 만약 frames 배열이 아직 여유공간이 있다면
            if (empty_idx != EMPTY_FRAME) {
                target = empty_idx;
            }
            // frames 배열이 꽉 차있어서 누군가를 SWAP OUT 해야 한다면
            else {
                int* values = (int*) malloc(sizeof(int) * frame_sz);
                for(int k = 0; k < frame_sz; k++) {
                    values[k] = additional[frames[k]];
                }
                
                int min = _min(values, frame_sz);
                target = _contains(additional, page_max+1, min);
                target = _contains(frames, frame_sz, target);
            }

            //target이 되는 자리에 해당 페이지를 frames에 SWAP IN한다.
            frames[target] = ref_arr[i];
            page_faults++;

            //모든 부가 비트들을 오른쪽시프트 하고, 요청된 페이지만 128을 더해준다.
            for(int j=0; j<page_max+1; j++) {
                additional[j] = additional[j]/2;
            }
            additional[ref_arr[i]] += 128;
        }

        //Page Fault가 발생하지 않았다면,
        //모든 페이지들의 additional bits를 한 번씩 오른쪽 Shift 해주고,
        //해당 페이지의 additional bits에만 128을 더해준다.
        else {
            for(int j=0; j<page_max+1; j++) {
                additional[j] = additional[j]/2;
            }
            additional[ref_arr[i]] += 128;
        }

        // Printing current states of frames
        printf("%d | ", ref_arr[i]);
        for(j=0; j<frame_sz; j++) {
            if (frames[j] == EMPTY_FRAME) printf(". ");
            else printf("%d ", frames[j]);
        }
        if (is_fault == -1) printf("(fault)");
        printf("\n");
    }

    return page_faults;
}

