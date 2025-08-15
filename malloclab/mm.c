/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "remain11-team",
    /* First member's full name */
    "remain11",
    /* First member's email address */
    "remain11@hhu.edu.cn",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1 << 10) // Extend heap by this amount

#define MAX(x, y) ((x) > (y) ? (x) : (y))


#define MIN(x, y) ((x) < (y) ? (x) : (y))

#define PACK(size, alloc) ((size) | (alloc)) // front and back

#define GET(p) (*(size_t*)(p))
#define PUT(p, val) (*(size_t*)(p) = (size_t)(val))

#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1) 

#define HDRP(bp) (bp - WSIZE)
#define FTRP(bp) (bp + GET_SIZE(HDRP(bp)) - DSIZE)
#define PRED(bp) (bp)
#define SUCC(bp) (bp + WSIZE)

#define NEXT_BLKP(bp) (bp + GET_SIZE(HDRP(bp)))
#define PREV_BLKP(bp) (bp - GET_SIZE(bp - DSIZE))

void *heap_listp;

void mm_check() {
    void *bp = heap_listp;
    printf("start mm_check\n");
    // 注意这里只能用HDRP
    for(bp = heap_listp; ;bp = NEXT_BLKP(bp)) {
        if (!GET_ALLOC(HDRP(bp)) || bp == heap_listp || GET_SIZE(HDRP(bp)) == 0){
            printf("bp = %p hdrp = %p ftrp = %p pred = %p succ = %p size = %d alloc = %d\n", 
            bp, HDRP(bp), FTRP(bp), (void *)GET(PRED(bp)), (void *)GET(SUCC(bp)), GET_SIZE(HDRP(bp)), GET_ALLOC(HDRP(bp)));
        } else {
            printf("bp = %p hdrp = %p ftrp = %p size = %d alloc = %d\n", 
            bp, HDRP(bp), FTRP(bp), GET_SIZE(HDRP(bp)), GET_ALLOC(HDRP(bp)));
        }  
        if (GET_SIZE(HDRP(bp)) == 0) break;
    }
    printf("\n");
    for(bp = heap_listp; ;bp = (void *)GET(SUCC(bp))) {
        printf("bp = %p hdrp = %p ftrp = %p pred = %p succ = %p size = %d alloc = %d\n", 
            bp, HDRP(bp), FTRP(bp), (void *)GET(PRED(bp)), (void *)GET(SUCC(bp)), GET_SIZE(HDRP(bp)), GET_ALLOC(HDRP(bp)));
        if (GET_SIZE(HDRP(bp)) == 0) break;
    }
    printf("\n");
}

static void *coalesce(void *bp) {
    void *prev_ftrp = FTRP(PREV_BLKP(bp));
    void *next_hdrp = HDRP(NEXT_BLKP(bp));
    void *newbp=bp;
    void *tempbp;
    size_t prev_alloc = GET_ALLOC(prev_ftrp);
    size_t next_alloc = GET_ALLOC(next_hdrp);
    size_t asize = GET_SIZE(HDRP(bp));
    size_t pack;

    if (prev_alloc && next_alloc) {
        return bp;
    } else if (prev_alloc && !next_alloc) {
        asize += GET_SIZE(next_hdrp);
        tempbp = (void *)GET(SUCC(NEXT_BLKP(bp)));
        PUT(SUCC(bp), tempbp);
        PUT(PRED(tempbp), bp);
        pack = PACK(asize, 0);
        PUT(HDRP(bp), pack);
        PUT(FTRP(bp), pack);
    } else if (!prev_alloc && next_alloc) {
        asize += GET_SIZE(prev_ftrp);
        newbp = PREV_BLKP(bp);
        tempbp = (void *)GET(SUCC(bp));
        PUT(SUCC(newbp), tempbp);
        PUT(PRED(GET(SUCC(bp))), newbp);
        pack = PACK(asize, 0);
        PUT(HDRP(newbp), pack);
        PUT(FTRP(newbp), pack);
    } else {
        asize += GET_SIZE(next_hdrp) + GET_SIZE(prev_ftrp);
        newbp = PREV_BLKP(bp);
        tempbp = (void *)GET(SUCC(NEXT_BLKP(bp)));
        PUT(SUCC(newbp), tempbp);
        // printf("tempbp = %p, pred(tempbp) = %p\n", tempbp, PRED(tempbp));
        PUT(PRED(tempbp), newbp);
        pack = PACK(asize, 0);
        PUT(HDRP(newbp), pack);
        PUT(FTRP(newbp), pack);
    }
    return newbp;
}

static void *extend_heap(size_t words) {
    void *bp;
    size_t asize;
    size_t pack;
    void *nextbp;

    asize = (words & 1 ? words + 1 : words) * WSIZE;
    if ((long)(bp = mem_sbrk(asize)) == -1) {
        return NULL;
    }
    bp = bp - DSIZE;
    pack = PACK(asize, 0);
    PUT(HDRP(bp), pack);
    PUT(FTRP(bp), pack);
    nextbp = NEXT_BLKP(bp);
    // printf("bp = %p nextbp = %p getsize(hdrp(bp)) = %d\n", bp, nextbp, GET(HDRP(bp)));
    PUT(HDRP(nextbp), PACK(0, 1));

    PUT(SUCC(bp), nextbp);
    PUT(PRED(nextbp), bp);
    PUT(SUCC(nextbp), 0);

    return coalesce(bp);
}
// 找到一个能放下asize字节的位置，使用最佳适配法
static void *find_fit(size_t asize) {
    void *bp = heap_listp;
    void *hdrp;
    void *ans = NULL;
    size_t ansasize = 1 << 30;
    size_t nowasize;
    // printf("%p\n", bp);
    do {
        // 注意这里只能用HDRP
        bp = (void *)GET(SUCC(bp));
        hdrp = HDRP(bp);
        nowasize = GET_SIZE(hdrp);
        if (nowasize >= asize && ansasize > nowasize) {
            // return bp;
            ans = bp;
            ansasize = nowasize;
        }
        // printf("%p\n", bp);
    } while (GET_SIZE(hdrp));
    return ans;
}
// 在bp放下newasize字节的块，有可能放下比newasize大的块
static void place(void *bp, size_t newasize) {
    void *nextbp;
    size_t asize = GET_SIZE(HDRP(bp));
    size_t pack;
    if (asize - newasize < 128) {
        pack = PACK(asize, 1);
        PUT(SUCC(GET(PRED(bp))), GET(SUCC(bp)));
        PUT(PRED(GET(SUCC(bp))), GET(PRED(bp)));
        PUT(HDRP(bp), pack);
        PUT(FTRP(bp), pack);
    } else {
        pack = PACK(newasize, 1);
        PUT(HDRP(bp), pack);
        PUT(FTRP(bp), pack);
        nextbp = NEXT_BLKP(bp);
        pack = PACK(asize - newasize, 0);
        PUT(HDRP(nextbp), pack);
        PUT(FTRP(nextbp), pack);

        PUT(SUCC(GET(PRED(bp))), nextbp);
        PUT(PRED(nextbp), GET(PRED(bp)));
        PUT(PRED(GET(SUCC(bp))), nextbp);
        PUT(SUCC(nextbp), GET(SUCC(bp)));
        // printf("newasize = %d, asize = %d, bp = %p, nextbp = %p, GET(SUCC(bp)) = %p\n", 
        //     newasize, asize,
        //     bp, nextbp, (void *)GET(SUCC(bp)));
        // assert((void *)GET(SUCC(nextbp)) != nextbp);
    }
}

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void) {
    size_t pack;
    // printf("pointer size is %d\n", sizeof(size_t*));
    if ((heap_listp = mem_sbrk(WSIZE << 3)) == (void *) -1) {
        return -1;
    }
    pack = PACK(16, 1);
    PUT(heap_listp, 0);     // 空白填充
    PUT(heap_listp + WSIZE, pack); // 第一个块的头部
    PUT(heap_listp + WSIZE * 2, 0);  // pred / bp
    PUT(heap_listp + WSIZE * 3, heap_listp + WSIZE * 6); 
    PUT(heap_listp + WSIZE * 4, pack); // 第一个块的尾部
    PUT(heap_listp + WSIZE * 5, 1); // 特殊块
    PUT(heap_listp + WSIZE * 6, heap_listp + WSIZE * 2); // pred / bp
    PUT(heap_listp + WSIZE * 7, 0); 

    heap_listp += DSIZE; // 指向第一个bp

    // 扩充
    if (extend_heap(CHUNKSIZE / WSIZE) == NULL) {
        return -1;
    }
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size) {
    size_t asize; // 对齐过后的blocksize
    size_t extendsize; // 申请的堆空间
    void *bp;

    if (size == 0) {
        return NULL;
    }

    // 上取整到最近对齐, 最低16字节
    asize = MAX(ALIGN(size + DSIZE), 16);

    if ((bp = find_fit(asize)) != NULL) {
        place(bp, asize);
        return bp;
    }
    // 未找到，申请更多堆空间
    extendsize = MAX(asize, CHUNKSIZE);
    // printf("未找到符合块，申请更多堆空间 %d\n", extendsize);
    // fflush(stdout);
    if ((bp = extend_heap(extendsize >> 2)) == NULL) {
        return NULL;
    }
    place(bp, asize);
    return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *bp) {
    void *hdrp = HDRP(bp);
    void *tempbp;
    size_t asize = GET_SIZE(hdrp);
    size_t pack = PACK(asize, 0);

    // assert(GET_ALLOC(HDRP(bp)) == 1);
    // printf("max = %p\n", mem_heap_hi);
    // printf("GET_SIZE(HDRP(tempbp)) = %d\n", GET_SIZE(HDRP(bp)));
    for (tempbp = bp; GET_SIZE(HDRP(tempbp)) != 0; tempbp = NEXT_BLKP(tempbp)) {
        if (!GET_ALLOC(HDRP(tempbp))) break;
        // printf("tempbp1 = %p GET_SIZE(HDRP(tempbp)) = %d\n", tempbp, GET_SIZE(HDRP(tempbp)));
    }
    PUT(SUCC(bp), tempbp);
    PUT(PRED(tempbp), bp);

    for (tempbp = bp; tempbp != heap_listp; tempbp = PREV_BLKP(tempbp)) {
        if (!GET_ALLOC(HDRP(tempbp))) break;
        // printf("tempbp2 = %p GET_SIZE(bp - 16) = %d\n", tempbp, GET_SIZE(bp - 16));
    }
    PUT(PRED(bp), tempbp);
    PUT(SUCC(tempbp), bp);

    PUT(hdrp, pack);
    PUT(FTRP(bp), pack);
    // mm_check();
    coalesce(bp);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t newsize) {
    void *oldptr = ptr, *nextptr, *newnextptr;
    void *prevptr; 
    void *bp;
    size_t asize, newasize, nextasize, prevasize;
    size_t pack;
    void *hdrp = HDRP(ptr);
    void *ftrp = FTRP(ptr);
    void *tempbp;
    
    asize = GET_SIZE(hdrp);
    newsize = ALIGN(newsize);
    newasize = newsize + DSIZE;
    // 如果相等
    if (newasize <= asize)  {
        return ptr;
    }
    // 如果修改后比原来小
    // if (newasize < asize) {
    //     pack = PACK(newasize, 1);
    //     PUT(hdrp, pack);
    //     PUT(ftrp, pack);
    //     nextptr = NEXT_BLKP(ptr);
    //     pack = PACK(asize - newasize, 0);
    //     PUT(HDRP(nextptr), pack);
    //     PUT(FTRP(nextptr), pack);
    //     coalesce(nextptr);
    //     return ptr;
    // }

    
    prevptr = PREV_BLKP(ptr);
    prevasize = GET(HDRP(prevptr));

    nextptr = NEXT_BLKP(ptr);
    nextasize = GET_SIZE(HDRP(nextptr));
    // 合并两边
    if (!GET_ALLOC(HDRP(prevptr)) && !GET_ALLOC(HDRP(nextptr)) && prevasize + nextasize + asize >= newasize) {
        
        PUT(SUCC(GET(PRED(prevptr))), GET(SUCC(nextptr)));
        PUT(PRED(GET(SUCC(nextptr))), GET(PRED(prevptr)));

        pack = PACK(prevasize + asize + nextasize, 1);

        PUT(HDRP(prevptr), pack);
        PUT(FTRP(prevptr), pack);

        memcpy(prevptr, ptr, asize - DSIZE);

        return prevptr;
    }

    // 合并前面的
    if (!GET_ALLOC(HDRP(prevptr)) && prevasize + asize >= newasize) {
        
        PUT(SUCC(GET(PRED(prevptr))), GET(SUCC(prevptr)));
        PUT(PRED(GET(SUCC(prevptr))), GET(PRED(prevptr)));

        pack = PACK(prevasize + asize, 1);

        PUT(HDRP(prevptr), pack);
        PUT(FTRP(prevptr), pack);

        memcpy(prevptr, ptr, asize - DSIZE);

        return prevptr;
    }
    // 合并后面的
    // 如果修改后比原来大
    if (!GET_ALLOC(HDRP(nextptr)) && nextasize + asize >= newasize) {
        // 两个块可以合并成一个
        if (1) {
            // printf("!!\n");
            tempbp = (void *)GET(SUCC(nextptr));
            // printf("tempbp = %p, ptr = %p\n", tempbp, ptr);
            pack = PACK(nextasize + asize, 1);

            PUT(SUCC(GET(PRED(nextptr))), tempbp);
            PUT(PRED(tempbp), GET(PRED(nextptr)));

            memset(ftrp, 0, nextasize);
            
            PUT(hdrp, pack);
            PUT(FTRP(ptr), pack);
            return ptr;
        }  else {
            // 这里发现分割后利用率会下降
            // 仍然是两个块
            // printf("!!!\n");
        
            // 注意要先把指针更改，否则后面的操作可能覆盖
            newnextptr = nextptr + (newasize - asize);

            tempbp = (void *)GET(PRED(nextptr));
            PUT(SUCC(tempbp), newnextptr);
            PUT(PRED(newnextptr), tempbp);

            tempbp = (void *)GET(SUCC(nextptr));
            PUT(PRED(tempbp), newnextptr);
            PUT(SUCC(newnextptr), tempbp);

            pack = PACK(newasize, 1);
            PUT(hdrp, pack);
            PUT(FTRP(ptr), pack);

            pack = PACK(nextasize + asize - newasize, 0);
            PUT(HDRP(newnextptr), pack);
            PUT(FTRP(newnextptr), pack);

            memset(ftrp, 0, newasize - asize);

            return ptr;
        }
        
    }
    // 重新分配
    bp = mm_malloc(newsize);
    newasize = GET_SIZE(HDRP(bp)); 
    if (bp == NULL){
        return NULL;
    }
    // mm_check();
    assert(MIN(asize, newasize) > 0);
    // mm_check();
    // printf("bp = %p, oldptr = %p, asize = %d, newasize = %d\n", bp, oldptr, asize, newasize);
    // printf("bp_end = %p, copy_end = %p\n", FTRP(bp) + 4, bp + MIN(asize - 16, newasize - 16));
    memcpy(bp, oldptr, MIN(asize, newasize) - DSIZE);
    mm_free(oldptr);
    // mm_check();
    return bp;
}