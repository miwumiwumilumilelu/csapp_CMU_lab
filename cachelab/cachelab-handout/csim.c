#include "cachelab.h"
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
/*
  用cache表示一个缓存 typedef struct cache
  用set，E(lines)，b来找具体cache_fuck,每个set包含多个E，每个E包含多个块
  用cache_fuck中有valid,tag,time_tamp表示具体块
  用二维数组的思路去初始化cache，每一个缓存类似于一个二位数组，数组的每一个元素就是缓存中的具体fuck
*/

typedef struct cache_fuck
{
    int valid;     //有效位
    int tag;       //标记位 
    int time_tamp;   //时间戳                    
} Cache_fuck;

typedef struct cache_{
    int S;
    int E;
    int B;
    Cache_fuck **fuck;
}Cache;

int hit_count = 0, miss_count = 0, eviction_count = 0; // 记录冲突不命中、缓存不命中
int verbose = 0;                                       //是否打印详细信息
char t[1000];
Cache *cache = NULL;

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

void free_Cache()
{
    int S = cache->S;
    for (int i = 0; i < S; i++)
    {
        free(cache->fuck[i]);
    }
    free(cache->fuck);
    free(cache);
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

//先判断 miss 或 hit
int get_index(int op_s,int op_tag)
{
	for (int i = 0;i < cache->E; i++)
	{
		if(cache->fuck[op_s][i].valid && cache->fuck[op_s][i].tag == op_tag)
			return i;
	}//lets fucking hit it
	return -1;
}

//当接收到-1后,两种情况:
//所有行都满了。那么就要用到上面得 LRU 进行选择驱逐
//组中有空行，只不过还未操作过，有效位为0，找到这个空行即可
//因此，设计一个判满的或找到指定空位函数:
int is_full(int op_s)
{
	for(int i = 0 ;i <cache->E ;i++)
	{
		if(cache->fuck[op_s][i].valid==0)
			return i;
	}
	return -1;
}

//扫描完成后得到i进行LRU更新函数的调用
void update_info(int op_tag, int op_s)
{
    int index = get_index(op_s, op_tag);
    if (index == -1)
    {
        miss_count++;
        if (verbose)
            printf("miss ");
        int i = is_full(op_s);
        if(i==-1){
            eviction_count++;
            if(verbose) printf("eviction");
            i = find_LRU(op_s);
        }
        update(i,op_s,op_tag);
    }
    else{
        hit_count++;
        if(verbose)
            printf("hit");
        update(index,op_s,op_tag);    
    }
}

//以上是主要架构代码
//----------------------------------------------------------------------

/*
 设计的数据结构解决了对 Cache 的操作问题，LRU 时间戳的实现解决了核心的驱逐问题，缓存扫描解决了对块中哪一列进行操作的问题，而应该对哪一块进行操作呢？接下来要解决的就是指令的解析问题了
*/


//指令解析:
//输入数据为operation address, size的形式，operation很容易获取，重要的是从address中分别获取我们需要的s和tag，address结构如下：
//int op_tag = address >> (s + b);
//int op_s = (address >> b) & ((unsigned)(-1) >> (8 * sizeof(unsigned) - s));


//则可以得get_trace 函数:
void get_trace(int s, int E, int b)
{
    FILE *pFile;
    pFile = fopen(t, "r");
    if (pFile == NULL)
    {
        exit(-1);
    }
    char identifier;
    unsigned address;
    int size;
    // Reading lines like " M 20,1" or "L 19,3"
    while (fscanf(pFile, " %c %x,%d", &identifier, &address, &size) > 0) // I读不进来,忽略---size没啥用
    {
        //想办法先得到标记位和组序号
        int op_tag = address >> (s + b);
        int op_s = (address >> b) & ((unsigned)(-1) >> (8 * sizeof(unsigned) - s));
        switch (identifier)
        {
        case 'M': //一次存储一次加载
            update_info(op_tag, op_s);
            update_info(op_tag, op_s);
            break;
        case 'L':
            update_info(op_tag, op_s);
            break;
        case 'S':
            update_info(op_tag, op_s);
            break;
        }
    }
    fclose(pFile);
}

void print_help()
{
    printf("** A Cache Simulator by Deconx\n");
    printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n");
    printf("Options:\n");
    printf("-h         Print this help message.\n");
    printf("-v         Optional verbose flag.\n");
    printf("-s <num>   Number of set index bits.\n");
    printf("-E <num>   Number of lines per set.\n");
    printf("-b <num>   Number of block offset bits.\n");
    printf("-t <file>  Trace file.\n\n\n");
    printf("Examples:\n");
    printf("linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n");
    printf("linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}

/*获取命令行参数*/
//我们使用getopt()函数来获取命令行参数的字符串形式，
//然后用atoi()转换为要用的参数，
//最后用switch语句跳转到对应功能块
int main(int argc, char *argv[])
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
    Init_Cache(s, E, b); //初始化一个cache
    get_trace(s, E, b);
    free_Cache();
    // printSummary(hit_count, miss_count, eviction_count)
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}

//:wq
//make clean
//make && ./test-csim
