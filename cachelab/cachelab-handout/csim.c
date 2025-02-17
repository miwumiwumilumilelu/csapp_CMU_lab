#include "cachelab.h"

typedef struct cache{
    int S;
    int E;
    int B;
    Cache_line **line;
}Cache;
//定义了Cache结构体

typedef struct cache_line
{
    int valid;     //有效位
    int tag;       //标记位
    int time_tamp; //时间戳
} Cache_line;
//用Cache表示一个缓存，它包括 S, B, E 等特征
//每一个缓存类似于一个二位数组，数组的每一个元素就是缓存中的行所以用一个line来表示这一信息

void Init_Cache(int s, int E, int b)
{
    int S = 1 << s;
    int B = 1 << b;
    cache = (Cache *)malloc(sizeof(Cache));
    cache->S = S;
    cache->E = E;
    cache->B = B;
    cache->line = (Cache_line **)malloc(sizeof(Cache_line *) * S);
    for (int i = 0; i < S; i++)
    {
        cache->line[i] = (Cache_line *)malloc(sizeof(Cache_line) * E);
        for (int j = 0; j < E; j++)
        {
            cache->line[i][j].valid = 0; //初始时，高速缓存是空的
            cache->line[i][j].tag = -1;
            cache->line[i][j].time_tamp = 0;
        }
    }
}
//这里的time_tamp表示时间戳，是LRU策略需要用到的特征。Cache 初始化如上



int main()
{
    printSummary(0, 0, 0);
    return 0;
}
