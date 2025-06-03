#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>

typedef unsigned long long ull;
typedef struct Line {
    int use;
    ull flag;
    int cnt;
}line;

int check(line cache[], ull p, int s, int E, int b) {
    ull s_index = p >> b & ((1 << s) - 1);
    ull flag = p >> (b + s);
    int is_find = 0;
    for (int i = s_index * E; i < (s_index + 1) * E; i++) {
        if (cache[i].use == 1) {
            if (flag == cache[i].flag) {
                is_find = 1;
                cache[i].cnt = 0;
            } else {
                cache[i].cnt++;
            }
        }
    }
    return is_find;
}

int get(line cache[], ull p, int s, int E, int b) {
    ull s_index = p >> b & ((1 << s) - 1);
    // ull flag = p >> (b + s);
    int cnt = -1, id = -1;
    for (int i = s_index * E; i < (s_index + 1) * E; i++) {
        if (cache[i].use == 0) {
            return i;
        } else {
            if (cnt < cache[i].cnt) {
                cnt = cache[i].cnt;
                id = i;
            }
        }
    }
    return id;
}

int main(int argc, char* argv[]) {
    char ch;
    int s = 0, E = 0, b = 0;
    char*filename;
    int verbose = 0;
    unsigned int num_hits = 0, num_misses = 0, num_evictions = 0;
    while ((ch = getopt(argc, argv, "s:E:b:t:v")) != -1) {
        switch (ch) {
            case 's':
                s = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 't':
                filename = optarg;
                // printf("opt is b, oprarg is: %s\n", optarg);
                break;
            case 'b':
                b = atoi(optarg);
                // printf("opt is b, oprarg is: %s\n", optarg);
                break;  
            case 'v':
                verbose = 1;
                break;
            case '?':
                printf("Wrong command\n");
                break;
        }
    }
    assert(filename);
    // printf("s = %d, E = %d, b = %d, t = %s, v = %d\n", s, E, b, filename, verbose);
    FILE*f = fopen(filename, "r");
    assert(f);
    char op[2];
    ull p = 0, len = 0;
    int S = 1 << s;
    line*cache = (line*) malloc(S * E * sizeof (line));
    while(~fscanf(f, "%s %llx,%llu", op, &p, &len)) {
        if (op[0] == 'I') continue;
        // printf("%s %llx %llu\n", op, p, len);
        int miss_flag = 0, eviction_flag = 0, hit_flag = 0;
        if (op[0] == 'L' || op[0] == 'M') {
            if (check(cache, p, s, E, b)) {
                hit_flag++;
            } else {
                miss_flag++;
                int id = get(cache, p, s, E, b);
                if (cache[id].use) {
                    eviction_flag++;
                } 
                cache[id] = (line){1, p >> (b + s), 0};
            }
        }

        if (op[0] == 'S' || op[0] == 'M') {
            if (check(cache, p, s, E, b)) {
                hit_flag++;
            } else {
                miss_flag++;
                int id = get(cache, p, s, E, b);
                if (cache[id].use) {
                    eviction_flag++;
                } 
                cache[id] = (line){1, p >> (b + s), 0};
            }
        }
        num_misses += miss_flag;
        num_evictions += eviction_flag;
        num_hits += hit_flag;
        if (verbose) {
            printf("%s %llx,%llu", op, p, len);
            if (miss_flag) {
                printf(" miss");
            }
            if (eviction_flag) {
                printf(" eviction");
            }
            if (hit_flag) {
                printf(" hit");
            }
            printf("\n");
        }
    }
    fclose(f);
    printSummary(num_hits, num_misses, num_evictions);
    return 0;
}
