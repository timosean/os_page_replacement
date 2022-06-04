#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define EMPTY_FRAME -1
#define MAX_PAGE 10

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

int stack[MAX_PAGE];
int global_page_max;

// Driver code
//  Usage: ./program {ref_arr_size} {page_max} {frame_size}
int main(int argc, char** argv) {
    if (argc < 4) return -1;

    int ref_arr_sz = atoi(argv[1]); //Reference String의 길이
    int page_max = atoi(argv[2]);   //최대 페이지 번호
    int frame_sz = atoi(argv[3]);   //프레임 사이즈

    //Initializing Stack
    for(int i=0; i<page_max; i++){
        stack[i] = -1;
    }

    global_page_max = page_max;

    int* ref = generate_ref_arr(ref_arr_sz, page_max);
    int testRef[] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1};

    int page_faults = lru(ref, ref_arr_sz, frame_sz);
    //int page_faults = lru(testRef, ref_arr_sz, frame_sz);
    printf("\nTotal Page Fault: %d\n", page_faults);
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

//Stack을 이용한 LRU Simulation
int lru(int* ref_arr, size_t ref_arr_sz, size_t frame_sz) {
    int i, j, is_fault, target;
    int stack_target, cascade_start;
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
            int empty_stack_idx = _contains(stack, global_page_max, -1);

            // frames에 비어있는 자리가 있다면, 해당 위치에 요청한 페이지 번호를 push한다.
            // 마찬가지로 stack도 비어있는 자리의 top에 요청한 페이지 번호를 push한다.
            if (empty_idx != -1) {
                target = empty_idx;
                stack_target = empty_stack_idx;
                stack[stack_target] = ref_arr[i];
            }
            // 그런데, 만약 frames가 이미 꽉차있다면, stack의 가장 밑에 있는 페이지 번호를 가지고 있는 frames의 인덱스를 찾고,
            // 그 자리를 요청한 페이지 번호로 대체한다. (대체하는 것 자체가 Swap Out)
            // 스택은 Swap out된 페이지가 있는 인덱스부터 스택의 데이터들을 모두 한 칸 씩 내리고
            // 스택의 top에 요청한 페이지 번호를 push한다.
            // 만약 이 페이지가 스택에 처음 push되는 것이라면 그냥 스택의 top에 push한다.
            else {
                target = stack[0];
                target = _contains(frames, frame_sz, target);

                //만약, 스택에 요청한 페이지 (Swap In의 대상) 정보를 가지고 있었다면 해당 페이지를 top으로 올리고 나머지를 한 칸 씩 내린다.
                //그렇지 않고 스택에 처음 들어온 페이지라면 그냥 스택의 top에 올린다.
                if(cascade_start = _contains(stack, global_page_max, ref_arr[i]) != -1){
                    cascade(stack, cascade_start, global_page_max);
                    stack[empty_stack_idx] = ref_arr[i];
                }
                else {
                    stack[empty_stack_idx] = ref_arr[i];
                }

                //그리고 swap out당한 페이지는 stack에서도 없앤다.
                cascade(stack, 0, global_page_max);
            }
            
            frames[target] = ref_arr[i];
            page_faults++;
        }

        //Page Fault가 발생하지 않았다면 (요청한 페이지 번호가 이미 frames에 있는 경우)
        //frames는 그대로 놔두고,
        //Stack에서는 해당 페이지가 위치한 인덱스부터 끝까지 한 칸씩 끌어내리고,
        //마지막으로, 요청한 페이지 번호를 stack의 top에 push한다.
        else {
            cascade(stack, _contains(stack, global_page_max, ref_arr[i]), global_page_max);
            int empty_stack_idx = _contains(stack, global_page_max, -1);
            stack[empty_stack_idx] = ref_arr[i];
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

