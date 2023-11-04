//yongqi liang #75181206
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include "string.h"

#define HEAP_SIZE 127

unsigned char heap[HEAP_SIZE];
int mode;
int token_length;
struct Element
{
    int playloadsize;
    char *status;
    int header;
    int start;
};

struct Element arr[HEAP_SIZE];

void update_block(int current_pos, int block_size, int block_status)
{
    if (block_size != 0)
    {
        for (unsigned int x = current_pos + 1; x < block_size - 2; x++)
        {
            heap[x] = 0;
        }

        char header = block_status == 1 ? block_size << 1 | 1 : block_size << 1 & -2;
        heap[current_pos] = header;
        heap[current_pos + block_size - 1] = header;
    }
}

void update_block_best_fit(int current_pos, int block_size, int block_status)
{
    if (block_size != 0)
    {
        for (unsigned int x = current_pos + 1; x < block_size - 2; x++)
        {
            heap[x] = 0;
        }

        char header = block_status == 1 ? block_size << 1 | 1 : block_size << 1 & -2;
        heap[current_pos] = header;
        heap[current_pos + block_size - 1] = header;
    }
}

int malloc_command(int size)
{
    int p = 0;
    char header;
    int block_size;
    int remain_size;
    int new_block_size = size + 2;
    int bestp = 0;
    if (mode == 0)
    {
        while (p < HEAP_SIZE)
        {
            header = heap[p];
            block_size = (unsigned char)header >> 1;
            remain_size = block_size - new_block_size;
            printf("remainning block %d\n", remain_size);
            //perfect fit
            if(remain_size == 0)
            {
                update_block(p, new_block_size, 1);
                update_block(p + new_block_size, remain_size, -2);
                return p + 1;
                break;
            }

            if (remain_size >= 3 && !((int)header & 1))
            {
                update_block(p, new_block_size, 1);
                update_block(p + new_block_size, remain_size, -2);
                return p + 1;
            }

                p += block_size;
        }
    }
    if (mode == 1)
    {
        int bestdiff = INT_MAX;
        while (p < HEAP_SIZE)
        {

            header = heap[p];
            block_size = (unsigned char)header >> 1;
            remain_size = block_size - new_block_size;
            if (block_size >= new_block_size && !((int)header & 1))
            {
                if (bestdiff > remain_size)
                {
                    bestdiff = remain_size;
                    bestp = p;
                }
            }
            // test bestp
            // printf("bestp is %d\n", bestp);
            p += block_size;
        }
        update_block_best_fit(bestp, new_block_size, 1);
        update_block_best_fit(bestp + new_block_size, bestdiff, -2);
        return bestp + 1;
    }
    printf("ERROR!!!! no memory can be allocated\n");
    return 0;
}

void mergeNeighborFreeSpace(int index, int current_position, int block_size)
{
    int new_block_size = block_size;
    if (index >= 2)
    {
        char previous_header = heap[index - 2];
        if (!((int)previous_header & 1))
        {
            int previous_size = (unsigned char)previous_header >> 1;
            current_position -= previous_size;
            new_block_size += previous_size;
        }
    }

    if (index + block_size - 1 < HEAP_SIZE)
    {
        char next_header = heap[index + block_size - 1];
        if (!((int)next_header & 1))
        {
            int next_size = (unsigned char)next_header >> 1;
            new_block_size += next_size;
        }
    }

    update_block(current_position, new_block_size, -2);
}
void sort_array_in_Descending_order(struct Element arr[], int size)
{
    int i, j;
    struct Element temp;
    for (i = 0; i < size; i++)
    {
        for (j = i + 1; j < size; j++)
        {
            if ((unsigned char)arr[i].header >> 1 < (unsigned char)arr[j].header >> 1)
            {
                temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }

            if ((unsigned char)arr[i].header >> 1 == (unsigned char)arr[j].header >> 1)
            {
                if (arr[i].start > arr[j].start)
                {
                    temp = arr[i];
                    arr[i] = arr[j];
                    arr[j] = temp;
                }
            }
        }
    }
}

// spilt the string into tokens
char **split_line(char line[])
{
    token_length = 0;
    int capacity = 16;

    char **tokens = malloc(capacity * sizeof(char *));

    char *delimiters = " \t\r\n";
    char *token = strtok(line, delimiters);

    while (token != NULL)
    {
        tokens[token_length] = token;
        token_length++;
        token = strtok(NULL, delimiters);
    }
    tokens[token_length] = NULL;
    return tokens;
}

int main(int argc, const char *argv[])
{
    mode = 0;
    if (argc == 2 && strcmp(argv[1], "BestFit") == 0)
    {
        mode = 1;
    }

    update_block(0, HEAP_SIZE, -2);
    char line[256];
    while (1) // while loop to get user input
    {
        printf("> ");
        fgets(line, sizeof(line), stdin);
        char **tokens = split_line(line);
        if (tokens != NULL)
        {
            if (!strcmp(tokens[0], "malloc"))
            {
                if (token_length == 2)
                {
                    int play_load_size = atoi(tokens[1]);
                    int blocksize = malloc_command(play_load_size);
                    printf("%d\n", blocksize);
                    continue;
                }
            }
            if (!strcmp(tokens[0], "free"))
            {
                if (token_length == 2)
                {
                    int index = atoi(tokens[1]);
                    char header = heap[index - 1];
                    int block_size = (unsigned char)header >> 1;
                    mergeNeighborFreeSpace(index, index - 1, block_size);
                    continue;
                }
            }
            if (!strcmp(tokens[0], "blocklist"))
            {
                int current_position = 0;
                int index = 0;

                while (current_position < HEAP_SIZE)
                {
                    char header = heap[current_position];
                    // start = p + 1;
                    // payload_size = get_block_size(header) - 2;
                    arr[index].header = header;
                    arr[index].status = ((int)header & 1) ? "allocated" : "free";
                    arr[index].playloadsize = ((unsigned char)header >> 1) - 2;
                    arr[index].start = current_position + 1;
                    // printf("%d-%d-%s\n", payload_size, start, status);
                    current_position += (unsigned char)header >> 1;
                    index++;
                }
                sort_array_in_Descending_order(arr, index);
                for (unsigned int x = 0; x < index; x++)
                {
                    printf("%d-%d-%s\n", arr[x].playloadsize, arr[x].start, arr[x].status);
                }
                continue;
            }
            if (!strcmp(tokens[0], "writemem"))
            {
                if (token_length == 3)
                {
                    int address_of_playLoad = atoi(tokens[1]);
                    char *write_data = tokens[2];
                    for (unsigned int x = 0; x < strlen(write_data); x++)
                    {
                        heap[address_of_playLoad] = write_data[x];
                        address_of_playLoad++;
                    }
                }
                continue;
            }
            if (!strcmp(tokens[0], "printmem"))
            {
                if (token_length == 3)
                {
                    int index = atoi(tokens[1]);
                    int print_block_size = atoi(tokens[2]);
                    for (unsigned int x = index; x < index + print_block_size; x++)
                    {
                        if (x != index + print_block_size - 1)
                        {
                            int ascii = heap[x];
                            printf("%d-", ascii);
                        }
                        else
                        {
                            int ascii = heap[x];
                            printf("%d", ascii);
                        }
                    }
                    printf("\n");
                }
                continue;
            }
            if (!strcmp(tokens[0], "quit"))
            {
                break;
                exit(0);
            }
        }
    }
    return 0;
}
