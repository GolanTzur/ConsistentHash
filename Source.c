#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <limits.h>

#define N 10000
#define SERVERS 100
#define EXPANSION 5 
#define _CRT_SECURE_NO_WARNINGS

typedef struct Server
{
	unsigned int id;
	unsigned int parentid;
	unsigned int phSize;
	unsigned int logsize;
	unsigned int* keys;
} Server;

void insertToServer(Server* server, unsigned int key);
void bubbleSortServers(Server* arr, int n);

void assignKeys(Server* servers) //servers  already sorted
{
	srand((unsigned int)time(NULL));
	for (int i = 0; i < N; i++)
	{
		int num = rand() % UINT_MAX;
		for (int j = 0; j < SERVERS; j++)
		{
			if (num <= servers[j].id)
			{
				insertToServer(&servers[j], num);
				break;
			}
		}
	}
	
}

void insertToServer(Server* server, unsigned int key)
{
	if(server->keys == NULL)
		server->keys = (unsigned int*)malloc(server->phSize * sizeof(unsigned int));
	if (server->logsize == server->phSize)
	{
		server->phSize *= 2;
		server->keys = (unsigned int*)realloc(server->keys, server->phSize * sizeof(unsigned int));
	}
	server->keys[server->logsize] = key;
	server->logsize++;
}

void deleteFromServer(Server* server, unsigned int key)
{
	for (int i = 0; i < server->logsize; i++)
	{
		if (server->keys[i] == key)
		{
			for (int j = i; j < server->logsize - 1; j++)
			{
				server->keys[j] = server->keys[j + 1];
			}
			server->logsize--;
			return;
		}
	}
}

Server* initServers()
{
	Server* servers = (Server*)malloc(SERVERS * sizeof(Server));
	srand((unsigned int)time(NULL));
	for (int i = 0; i < SERVERS; i++)
	{
		servers[i].phSize = 2;
		servers[i].logsize = 0;
		servers[i].parentid = -1;
		servers[i].id = rand() % UINT_MAX;
		servers[i].keys = NULL;
	}
	return servers;
}

void expandServers(Server** servers)
{
	*servers = (Server*)realloc(*servers, (SERVERS*EXPANSION) * sizeof(Server));
	for (int i = SERVERS; i < SERVERS*EXPANSION; i++)
	{
		(*servers)[i].phSize = 2;
		(*servers)[i].logsize = 0;
		(*servers)[i].parentid = i % ((*servers) + (i%SERVERS))->id;
		(*servers)[i].id = rand() % UINT_MAX;
		(*servers)[i].keys = NULL;
	}

	//Re sort the servers after expansion
	bubbleSortServers(*servers, SERVERS * EXPANSION);

	//Re assign the keys to the servers after expansion
	for (int i = 0; i < SERVERS * EXPANSION; i++)
	{
		for (int j = 0; j < (*servers)[i].logsize; j++)
		{
			unsigned int key = (*servers)[i].keys[j];
			deleteFromServer(&(*servers)[i], key);
			for (int k = 0; k < SERVERS * EXPANSION; k++)
			{
				if (key <= (*servers)[k].id)
				{
					insertToServer(&(*servers)[k], key);
					break;
				}
			}
		}
	}
}

void bubbleSortServers(Server* arr,int n)
{
	for (int i = 0; i < n - 1; i++)
	{
		for (int j = 0; j < n - i - 1; j++)
		{
			if ((arr+j)->id > (arr + j+1)->id)
			{
				unsigned int temp = (arr + j)->id;
				arr[j].id = arr[j + 1].id;
				arr[j + 1].id = temp;
			}
		}
	}
}

void bubbleSort(unsigned int* arr, int n)
{
	for (int i = 0; i < n - 1; i++)
	{
		for (int j = 0; j < n - i - 1; j++)
		{
			if (arr[j] > arr[j + 1])
			{
				unsigned int temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
			}
		}
	}
}

int main()
{
	Server* servers = initServers();
	bubbleSortServers(servers,SERVERS);
	assignKeys(servers);

	int avgload,medianload,twentyfivepercentile,seventyfivepercentile;
	avgload = 0;
	for (int i = 0; i < SERVERS; i++)
	{
		avgload += servers[i].logsize;
	}
	avgload /= SERVERS;

	//calculate median
	int* loads = (int*)malloc(SERVERS * sizeof(int));
	for (int i = 0; i < SERVERS; i++)
	{
		loads[i] = servers[i].logsize;
	}
	bubbleSort(loads, SERVERS);
	medianload = loads[SERVERS / 2];
	twentyfivepercentile = loads[SERVERS / 4];
	seventyfivepercentile = loads[3 * SERVERS / 4];

	//print the results
	printf("Average Load: %d\n", avgload);
	printf("Median Load: %d\n", medianload);
	printf("25th Percentile Load: %d\n", twentyfivepercentile);
	printf("75th Percentile Load: %d\n", seventyfivepercentile);


	//expand the servers and re calculate the loads
	expandServers(&servers);
	

	//Calculate loads for a server, and then statistics
	for (int i = 0; i < SERVERS * EXPANSION; i++)
	{
		int loadsCount = 0;
		if (servers[i].parentid == -1)
		{
			loads[loadsCount] = servers[i].logsize;
			int currId = servers[i].id;
			for (int j = 0; j < SERVERS * EXPANSION; j++)
			{
				if (servers[j].parentid == currId)
				{
					loads[loadsCount] += servers[j].logsize;
				}
			}
			loadsCount++;
		}
	}

	avgload = 0;
	for (int i = 0; i < SERVERS; i++)
	{
		avgload += loads[i];
	}
	avgload /= (SERVERS);

	bubbleSort(loads, SERVERS);
	medianload = loads[(SERVERS) / 2];
	twentyfivepercentile = loads[(SERVERS) / 4];
	seventyfivepercentile = loads[3 * (SERVERS) / 4];

	//print the results
	printf("After Expansion:\n");
	printf("Average Load: %d\n", avgload);
	printf("Median Load: %d\n", medianload);
	printf("25th Percentile Load: %d\n", twentyfivepercentile);
	printf("75th Percentile Load: %d\n", seventyfivepercentile);
	
	return 0;
}