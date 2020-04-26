#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define ROW_DIM 16
#define COL_DIM 16

typedef struct
{
    int block;
    int set;
    int valid;
    //int dirty;
    int tag;
    int time_modified;
} Line;

typedef struct
{
    Line *lines;
} Set;

typedef struct
{
    Set *sets;
    int cache_size;
    int block_size;
    int set_count;
    int associativity;
} Cache;

typedef int array[ROW_DIM][COL_DIM];

typedef struct
{
    unsigned int hits;
    unsigned int misses;
    unsigned int time;
} Cache_status;

array src_hm, dst_hm;

Cache create_cache(int associativity, int cache_size, int block_size)
{
    Cache newCache;
    Set set;
    Line line;
    int set_count = cache_size / (associativity * block_size);
    int i;
    int j;
    newCache.cache_size = cache_size;
    newCache.block_size = block_size;
    newCache.set_count = set_count;
    newCache.associativity = associativity;
    newCache.sets = (Set *)malloc(sizeof(Set) * set_count);
    for (i = 0; i < set_count; i++)
    {
        set.lines = (Line *)malloc(sizeof(Line) * associativity);
        newCache.sets[i] = set;

        for (j = 0; j < associativity; j++)
        {
            line.time_modified = 0;
            line.valid = 0;
            //line.dirty = 0;
            line.tag = 0;
            set.lines[j] = line;
        }
    }
    return newCache;
}

void free_cache(Cache freecache)
{
    int i;
    int set_count = freecache.set_count;
    for (i = 0; i < set_count; i++)
    {
        Set set = freecache.sets[i];

        if (set.lines != NULL)
        {
            free(set.lines);
        }
    }
    if (freecache.sets != NULL)
    {
        free(freecache.sets);
    }
}

Line line_initialized(int block_size, int set_count, int address)
{
    Line line;
    int bbits = log(block_size >> 2) / log(2);
    int sbits = log(set_count) / log(2);
    int blockMask = (block_size >> 2) - 1;
    int setMask = (set_count - 1) << bbits;
    int tagMask = -1 ^ setMask ^ blockMask;
    printf("Address = %d\n", address);
    printf("Block bits = %d\n", bbits);
    printf("Sets bits = %d\n", sbits);
    printf("BlockMask = %d\n", blockMask);
    // printf("Set Mask = %d\n",setMask);
    // printf("Tag Mask = %d\n",tagMask);

    line.block = address & blockMask;
    line.set = (address & setMask) >> bbits;
    line.tag = (((address & tagMask) >> bbits) >> sbits);

    printf("Block Index = %d\n", line.block);
    printf("Set Index = %d\n", line.set);
    printf("Tag Index = %d\n", line.tag);
    printf("---------------------------------------\n");
    return line;
}

Cache cache_simulation(Cache cache, Line dataset, int data, Cache_status *track, int r, int c, int w)
{
    //Line dataset;
    int i, hit_indicator = 0, lru_index = 0;
    //dataset = line_initialized(cache.block_size, cache.set_count, data);// Need to be fixed
    Set set = cache.sets[dataset.set];

    if (cache.associativity == 1) {
        int offset = cache.block_size >> 2;
        for (i = 0; i < offset; i++)
        {
            if (set.lines[i].valid)
            {
                if (dataset.tag == set.lines[i].tag)
                {
                    /* cache hit case */
                    hit_indicator = 1;
                    if (w == 0)
                    {
                        src_hm[r][c] = 1;
                    }
                    else
                    {
                        dst_hm[r][c] = 1;
                    }
                    track->hits++;
                    set.lines[i].time_modified = track->time;
                    track->time++;
                    break;
                }
            }
        }

    }
    else {
        for (i = 0; i < cache.associativity; i++)
        {
            if (set.lines[i].valid)
            {
                if (dataset.tag == set.lines[i].tag)
                {
                    /* cache hit case */
                    hit_indicator = 1;
                    if (w == 0)
                    {
                        src_hm[r][c] = 1;
                    }
                    else
                    {
                        dst_hm[r][c] = 1;
                    }
                    track->hits++;
                    set.lines[i].time_modified = track->time;
                    track->time++;
                    break;
                }
            }
        }
    }
    
    if (!hit_indicator)
    {
        /* cache miss case */
        track->misses++;
        if (w == 0)
        {
            src_hm[r][c] = 0;
        }
        else
        {
            dst_hm[r][c] = 0;
        }
        int acc = 0;
        int empty_flag, empty_index;
        for (i = 0; i < cache.associativity; i++)
        {
            if (set.lines[i].valid == 0)
            {
                empty_flag = 1;
                empty_index = 1;
                break;
            }
        }
        if (!empty_flag)
        {
            int least_time = set.lines[0].time_modified;
            for (i = 0; i < cache.associativity - 1; i++)
            {
                if (least_time >= set.lines[i + 1].time_modified)
                {
                    least_time = set.lines[i + 1].time_modified;
                    lru_index = i + 1;
                }
            }
            Line line3 = set.lines[lru_index];
            line3.tag = dataset.tag;
            line3.time_modified = track->time;
            track->time++;
        }
        else
        {
            set.lines[empty_index].tag = dataset.tag;
            set.lines[empty_index].valid = 1;
            set.lines[empty_index].time_modified = track->time; //need modification
            track->time++;
        }
    }
    return cache;
}

int getAddress(int w, int r, int c)
{
    // 256 = 16 * 16 Matrix
    int address = w * (ROW_DIM * COL_DIM) + (r * ROW_DIM + c);
    // printf("%d\n", address);
    return address;
}
/* testing functions */

void transpose1(array dst, array src, Cache cache, Cache_status *track)
{
    int i, j, acc;
    for (i = 0; i < ROW_DIM; i++)
    {
        for (j = 0; j < COL_DIM; j++)
        {
            acc = src[i][j];
            //printf("%d",&src[i][j]);
            int k = src[i][j];
            int address1 = getAddress(0, i, j);

            Line line1 = line_initialized(cache.block_size, cache.set_count, address1);
            cache = cache_simulation(cache, line1, src[i][j], track, i, j, 0);
            dst[j][i] = acc;
            //printf("%d",&dst[j][i]);
            int l = dst[j][i];
            int address2 = getAddress(1, j, i);
            Line line2 = line_initialized(cache.block_size, cache.set_count, address2);
            
            cache = cache_simulation(cache, line2, dst[j][i], track, j, i, 1);

            printf("***************************************\n");
        }
    }
}

void initialize_array(array src)
{
    int i, j;
    for (i = 0; i < ROW_DIM; i++)
    {
        for (j = 0; j < COL_DIM; j++)
        {
            src[i][j] = rand() % 2;
        }
    }
}

void printArray(array arr)
{
    int i, j;
    for (i = 0; i < ROW_DIM; i++)
    {
        for (j = 0; j < COL_DIM; j++)
        {
            printf("%d ", arr[i][j]);
        }
        printf("\n");
    }
}

int isPow2(int x) {
    int i;
    for (i = 0; i < 31; i++) {
        if (x == 1 << i)
            return 1;
    }
    return 0;
}

int validCacheSize(int cachesize) {
    if (cachesize == 0)
        return 0;
    if (isPow2(cachesize)) {
        return cachesize;
    }
    return 0;
}

int validBlockSize(int blocksize) {
    if (blocksize == 0)
        return 0;
    if (isPow2(blocksize))
        return blocksize;
    return 0;
}



int check_valid(int cache_size, int block_size, int associativity) {
    
    cache_size = validCacheSize(cache_size);
    if(!cache_size)
        return 0;
    block_size = validBlockSize(block_size);
    if (!block_size || block_size > cache_size)
        return 0;
    return 1;
}

int main()
{
    printf("**************** Start Main Program ****************\n");
    array src, dst;
    Cache_status *track;
    track = (Cache_status *)malloc(sizeof(Cache_status));
    track->hits = 0;
    track->misses = 0;
    track->time = 0;

    int cache_size, block_size, associativity;
    char argv[20];

    printf("Enter Cache Size (must is 4,8,16,32,64, 128, 256) :\n");
    scanf("%s", argv);
    cache_size = atoi(argv);

    printf("Enter Block Byte (must is 4, 8, 16) :\n");
    scanf("%s", argv);
    block_size = atoi(argv);

    

    printf("Enter associativity (must is 1, 2, 4) :\n");
    scanf("%s", argv);
    associativity = atoi(argv);

    int valid = check_valid(cache_size, block_size, associativity);
    if (!valid) {
        printf("Invalid Parameter!!! Please check again!\n");
        return 0;
    }
        
    Cache new_cache = create_cache(associativity, cache_size, block_size); //cache 128, block 16

                                                                           // Initialize Src Array
    initialize_array(src);


    // Transpose Matrix src to dst
    transpose1(dst, src, new_cache, track);
    

    printf("---------------- print Src hit or miss Matrix-------------\n");
    printArray(src_hm);

    printf("---------------- print Dst hit or miss Matrix -------------\n");
    printArray(dst_hm);

    // Print Cache Information
    printf("Cache Size = %d Block size = %d Set Number = %d\n", new_cache.cache_size, new_cache.block_size, new_cache.set_count);
    printf("%d-Way Associative Cache: hits = %d, misses = %d\n", new_cache.associativity, track->hits, track->misses);
    free_cache(new_cache);
    return 0;
}
