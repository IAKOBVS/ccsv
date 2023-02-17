#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define MIN_STR_LEN 128

struct Value {
	char *value;
	size_t valueLen;
	size_t valueSize;
};

struct Record {
	struct Value *values;
	size_t valuesQ;
};

struct Key {
	char *key;
	size_t keySize;
	size_t keyLen;
};

struct Data {
	struct Key *keys;
	size_t keysQ;
	struct Record *records;
	size_t recordsQ;
};

void dataFree(struct Data *data)
{
	for ( ; data->recordsQ > -1; --data->recordsQ) {
		for (size_t iV = 0; iV < data->records->valuesQ; ++iV)
			free(data->records[data->recordsQ].values[iV].value);
		free(data->records[data->recordsQ].values);
	}
	free(data->records);
	for ( ; data->keysQ > -1; --data->keysQ)
		free(data->keys[data->keysQ].key);
	free(data->keys);
	perror("");
}

int dataInit(struct Data *data, size_t keysQ, size_t recordsQ, size_t valuesQ)
{
	data->keysQ = 0;
	data->recordsQ = 0;
	if ((data->records = malloc(recordsQ * sizeof(struct Record))));
	else
		goto ERROR;
	data->records->valuesQ = 0;
	if ((data->keys = malloc(keysQ * sizeof(struct Key))));
	else goto ERROR_FREE;
	for ( ; data->keysQ < keysQ; ++data->keysQ) {
		if ((data->keys[data->keysQ].key = malloc(MIN_STR_LEN)));
		else goto ERROR_FREE;
		data->keys[data->keysQ].keySize = MIN_STR_LEN;
		data->keys[data->keysQ].keyLen = 0;
	}
	for ( ; data->recordsQ < recordsQ; ++data->recordsQ) {
		if ((data->records[data->recordsQ].values = malloc(valuesQ * sizeof(struct Value))));
		else goto ERROR_FREE;
		data->records[data->recordsQ].valuesQ = valuesQ;
		for ( ; data->records->valuesQ < valuesQ; ++data->records->valuesQ) {
			if ((data->records[data->recordsQ].values[data->records->valuesQ].value = malloc(MIN_STR_LEN)));
			else goto ERROR_FREE;
			data->records[data->recordsQ].values[data->records->valuesQ].valueSize = MIN_STR_LEN;
			data->records[data->recordsQ].values[data->records->valuesQ].valueLen = 0;
		}
	}
	return 1;

ERROR_FREE:
	dataFree(data);
ERROR:
	perror("");
	return 0;
}


int main()
{
	struct Data data;
	dataInit(&data, 10, 10, 10);
}
