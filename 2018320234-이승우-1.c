#include <stdio.h>
#include <stdlib.h>

#define EMPTY_FRAME -1

int* generate_ref_arr(size_t sz, size_t page_max);
int lru(int* ref_arr, size_t ref_arr_sz, size_t frame_sz);


int _contains(int* arr, size_t sz, int target) {
    int i = 0;
    for (i=0; i < sz; i++) {
        if (arr[i] == target) return i;
    }
    return -1;
}

void cascade(int* arr, int start, int end) {

    for(int i=start; i<end-1; i++) {
        arr[i] = arr[i+1];
    }

    //다 끌어내리고 마지막 top은 -1로 남겨둠.
    arr[end-1] = -1;
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

    //int page_faults = lru(testRef, ref_arr_sz, frame_sz);
    int page_faults = lru(ref, ref_arr_sz, frame_sz);
    printf("\nTotal Page Fault: %d\n", page_faults);
    free(ref);

    return 0;
}

int* generate_ref_arr(size_t sz, size_t page_max) {
    int* ref_arr = (int*) malloc(sizeof(int) * sz);
    
    // TODO :: Generate Random Refernece String
    for(int i=0; i < sz; i++) {
        //0부터 page_max 사이의 숫자를 랜덤으로 뽑아서 Reference String에 넣는다.
        ref_arr[i] = rand() % (page_max + 1);
    }
    return ref_arr;
}

//Stack을 이용한 LRU Simulation
int lru(int* ref_arr, size_t ref_arr_sz, size_t frame_sz) {
    int i, j, is_fault, target;
    int page_faults = 0;

    // Initializing frames (프레임 사이즈만큼 프레임들을 생성하고 모두 EMPTY_FRAME으로 초기화시킨다.)
    int* frames = (int*) malloc(sizeof(int) * frame_sz);
    for (i=0; i < frame_sz; i++) frames[i] = -1;

    // TODO
    // Iterating Reference String
    for (i=0; i<ref_arr_sz; i++) {
        //공통 출력과제
        printf("%d | ", ref_arr[i]);

        is_fault = _contains(frames, frame_sz, ref_arr[i]);

        // Miss (page fault occurred)
        if (is_fault == -1) {
            int empty_idx = _contains(frames, frame_sz, EMPTY_FRAME);

            // 스택에 비어있는 자리가 있다면, 해당 위치에 요청한 페이지 번호를 push한다.
            if (empty_idx != -1) {
                target = empty_idx;
            }
            // 그런데, 만약 스택이 이미 꽉차있다면, Bottom에 있는 페이지를 Swap Out시키고 (실제 구현상으로는 명시적으로 하진 않음)
            // 스택의 데이터들을 모두 한 칸 씩 내리고, (이 과정에서 이미 Swap Out이 포함)
            // 스택의 top에 요청한 페이지 번호를 push한다.
            else {
                cascade(frames, 0, frame_sz);
                target = frame_sz - 1;
            }

            frames[target] = ref_arr[i];
            page_faults++;
        }

        //Page Fault가 발생하지 않았다면 (요청한 페이지 번호가 이미 Stack에 있는 경우)
        //해당 페이지의 인덱스부터 끝까지 한 칸씩 끌어내리고,
        //마지막에 요청한 페이지 번호를 stack의 top에 push한다.
        else {
            cascade(frames, is_fault, frame_sz);
            int empty_idx = _contains(frames, frame_sz, EMPTY_FRAME);
            frames[empty_idx] = ref_arr[i];
        }

        //공통 출력과제
        for(int k=0; k<frame_sz; k++) {
            if(frames[k] == -1) {
                printf(". ");
            }
            else {
                printf("%d ", frames[k]);
            }
        }
        if(is_fault == -1) printf("(fault)");
        printf("\n");
    }

    return page_faults;
}

