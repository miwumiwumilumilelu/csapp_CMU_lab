#include "cachelab.h"

/*
  用cache表示一个缓存 typedef struct cache
  用set，E(lines)，b来找具体cache_fuck,每个set包含多个E，每个E包含多个块
  用cache_fuck中有valid,tag,time_tamp表示具体块
  用二维数组的思路去初始化cache，每一个缓存类似于一个二位数组，数组的每一个元素就是缓存中的具体fuck
*/

typedef struct cache{
    int S;
    int E;
    int B;
    Cache_fuck **fuck;
}Cache;

typedef struct Cache_fuck
{
    int valid;     //有效位
    int tag;       //标记位
    int time_tamp; //时间戳
} Cache_fuck;

void Init_Cache(int s, int E, int b)
{
    int S = 1 << s; //S=2^s
    int B = 1 << b; //B=2^b
    cache = (Cache *)malloc(sizeof(Cache)); //分配内存
    cache->S = S;
    cache->E = E;
    cache->B = B;
    cache->fuck = (Cache_fuck **)malloc(sizeof(Cache_fuck *) * S);
    for (int i = 0; i < S; i++)
    {
        cache->fuck[i] = (Cache_fuck *)malloc(sizeof(Cache_fuck) * E);
        for (int j = 0; j < E; j++)
        {
            cache->fuck[i][j].valid = 0; //初始时，高速缓存是空的
            cache->fuck[i][j].tag = -1;
            cache->fuck[i][j].time_tamp = 0;
        }
    }
}

/*
-----------------------
LRU（Least Recently Used，最近最少使用）是一种常用的缓存替换策略。它的核心思想是：当缓存空间不足时，优先淘汰最久未被使用的数据。LRU 算法基于时间局部性原理，即最近被访问的数据很可能在不久的将来再次被访问。
-----------------------
LRU 的工作原理:
缓存中的数据通常以键值对（key-value）的形式存储。
每次访问一个数据时，将该数据标记为“最近使用”。
当缓存已满且需要插入新数据时，淘汰最久未被使用的数据。
-----------------------
LRU 的实现方式
LRU 算法可以通过多种数据结构实现，常见的实现方式包括：
哈希表 + 双向链表：
哈希表用于快速查找数据。
双向链表用于维护数据的访问顺序，链表头部是最近使用的数据，尾部是最久未使用的数据
队列 + 哈希表：
队列用于维护访问顺序。
哈希表用于快速查找数据。
-----------------------
*/
void update(int i, int op_s, int op_tag){
    cache->fuck[op_s][i].valid=1;
    cache->fuck[op_s][i].tag = op_tag;
    for(int k = 0; k < cache->E; k++)
        if(cache->fuck[op_s][k].valid==1)
            cache->fuck[op_s][k].time_tamp++;
    cache->fuck[op_s][i].time_tamp = 0;
}
//这段代码在找到要进行的操作行后调用（无论是不命中还是命中，还是驱逐后）。前两行是对有效位和标志位的设置，与时间戳无关，主要关注后几行：
//-------------------------------
//遍历组中每一fuck，并将它们的值加1，也就是说每一行在进行一次操作后时间戳都会变大，表示它离最后操作的时间变久
//将本次操作的fuck时间戳设置为最小，也就是0

//找最大时间戳的fuck
int find_LRU(int op_s)
{
    int max_index = 0;
    int max_stamp = 0;
    for(int i = 0; i < cache->E; i++){
        if(cache->fuck[op_s][i].time_tamp > max_stamp){
            max_stamp = cache->fuck[op_s][i].time_tamp;
            max_index = i;
        }
    }//其核心逻辑是遍历集合内的所有缓存行，找到时间戳（time_tamp）最大的行
    return max_index;
}



/*获取命令行参数*/
//我们使用getopt()函数来获取命令行参数的字符串形式，
//然后用atoi()转换为要用的参数，
//最后用switch语句跳转到对应功能块
int main()
{
    char opt;
    int s, E, b;
    /*
     * s:S=2^s是组的个数
     * E:每组中有多少行
     * b:B=2^b每个缓冲块的字节数
     */
    while (-1 != (opt = getopt(argc, argv, "hvs:E:b:t:")))
    {
        switch (opt)
        {
        case 'h':
            print_help();
            exit(0);
        case 'v':
            verbose = 1;
            break;
        case 's':
            s = atoi(optarg);
            break;
        case 'E':
            E = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 't':
            strcpy(t, optarg);
            break;
        default:
            print_help();
            exit(-1);
        }
    }
    Init_Cache(s, E, b);
    get_trace(s, E, b);
    free_Cache();
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}
