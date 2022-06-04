#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define EMPTY_FRAME -1

int* generate_ref_arr(size_t sz, size_t page_max);
int lru(int* ref_arr, size_t ref_arr_sz, size_t frame_sz, size_t page_max);

/*------------- LinkedList 기반 Circular Queue의 구현 --------------------------*/
typedef struct __node {
    int pagenum;
    int rbit;
    struct __node * next;
}Node;

typedef struct _cq {
    Node* front;
    Node* rear;
    Node* cur;
}Queue;

void queueInit(Queue* pq) {
    pq->front = NULL;
    pq->rear = NULL;
    pq->cur = NULL;
}

int isEmpty(Queue* pq) {
    if(pq->front == NULL)
        return 1;
    else 
        return 0;
}

void enqueue(Queue* pq, int pagenum, int rbit) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->next = pq->front;  //Circular Queue이므로 새로 만들어진 노드의 next 포인터가 다시 큐의 처음을 가리켜야 한다.
    newNode->pagenum = pagenum;
    newNode->rbit = rbit;

    if(isEmpty(pq)) {
        pq->front = newNode;
        pq->rear = newNode;
        pq->cur = newNode;
    }
    else {
        pq->rear->next = newNode;
        pq->rear = newNode;
    }
}

//Reference Bit가 0인 페이지를 찾아가는 과정.
//지나가는 Reference Bit 1들은 모두 0으로 바꿔주고,
//처음 Reference Bit가 0인 페이지가 Victim이 되며, 해당 페이지를 새로운 페이지로 replace하고, Bit도 1로 바꿔준다.
int moveNext(Queue* pq, int newpage) {
    int ret;

    //printf("현재 cur PAGE는 %d이다\n", pq->cur->pagenum);

    while(pq->cur->rbit) {
        pq->cur->rbit = 0;
        pq->cur = pq->cur->next;
    }

    //printf("victim은 %d 으로 정해졌다.\n", pq->cur->pagenum);

    ret = pq->cur->pagenum;

    pq->cur->pagenum = newpage;
    pq->cur->rbit = 1;

    return ret;
}

//Page Fault가 발생하지 않았을 경우 요청한 페이지 노드에 대한 Reference Bit를 1로 변경시킨다.
void updateBit(Queue* pq, int targetPage) {
    Node* temp;
    temp = pq->front;

    while(temp->pagenum != targetPage)
        temp = temp->next;

    temp->rbit = 1;
    //printf("Page %d의 Reference Bit를 1로 바꿔줌.\n", targetPage);
}

/*--------------------------------------------------------------------------------*/

int _contains(int* arr, size_t sz, int target) {
    int i = 0;
    for (i=0; i < sz; i++) {
        if (arr[i] == target) return i;
    }
    return -1;
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

    int page_faults = lru(testRef, ref_arr_sz, frame_sz, page_max);
    //int page_faults = lru(ref, ref_arr_sz, frame_sz, page_max);
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
int lru(int* ref_arr, size_t ref_arr_sz, size_t frame_sz, size_t page_max) {
    int i, j, is_fault;
    int page_faults = 0;
    int target = 0;
    int victim;

    Queue myqueue;
    queueInit(&myqueue);
    
    // Initializing frames (프레임 사이즈만큼 프레임들을 생성하고 모두 EMPTY_FRAME으로 초기화시킨다.)
    int* frames = (int*) malloc(sizeof(int) * frame_sz);
    for (i=0; i<frame_sz; i++) frames[i] = -1;

    // TODO
    // Iterating Reference String
    for(i=0; i<ref_arr_sz; i++) {
        is_fault = _contains(frames, frame_sz, ref_arr[i]);

        // Miss (page fault occurred)
        if (is_fault == -1) {
            int empty_idx = _contains(frames, frame_sz, EMPTY_FRAME);

            //만약 완전 처음 데이터라면, frames배열의 첫번째에 요청 페이지를 넣고,
            //큐에는 (요청 페이지 번호, 1) 데이터를 삽입한다.
            if(isEmpty(&myqueue)) {
                target = empty_idx;
                frames[target] = ref_arr[i];
                enqueue(&myqueue, ref_arr[i], 1);
            }

            //큐에 데이터가 있는 상태이고, frames 배열에 여유공간이 "있다면"
            //그 여유공간에 요청 페이지를 넣고,
            //큐에는 (요청 페이지 번호, 1) 데이터를 삽입한다.
            else {
                if(empty_idx != -1) {
                    frames[empty_idx] = ref_arr[i];
                    enqueue(&myqueue, ref_arr[i], 1);
                }

                //만약 frames 배열이 이미 꽉차있는 상태라면,
                //큐에서 가장 처음 0이 되는 노드의 페이지번호를 찾아서, 요청된 새로운 페이지로 replace한다.
                //frames에서도 해당 페이지로 replace해주어야 한다.
                else{
                    //printf("현재 cur이 가리키는 페이지 = %d\n", myqueue.cur->pagenum);
                    victim = moveNext(&myqueue, ref_arr[i]);
                    target = _contains(frames, frame_sz, victim);
                    frames[target] = ref_arr[i];
                }
            }

            page_faults++;
        }

        // Page Fault가 발생하지 않았다면,
        // 요청한 페이지에 대한 Reference Bit를 1로 바꿔주어야 한다.
        else {
            updateBit(&myqueue, ref_arr[i]);
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

