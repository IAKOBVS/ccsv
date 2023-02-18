#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "/home/james/c/nix.c/nix.h"
#include "/home/james/c/jArray/jarr.h"
#include "/home/james/c/jString/jstr.h"

#define FILENAME "/tmp/stocks-nix/Balancepos20230131.txt"

#define MIN_STR_LEN 256

#if (defined(__GNUC__) && (__GNUC__ >= 3)) || (defined(__clang__) && __has_builtin(__builtin_expect))
  #define likely(x) __builtin_expect(!!(x), 1)
  #define unlikely(x) __builtin_expect(!!(x), 0)
#else
  #define likely(x) (x)
  #define unlikely(x) (x)
#endif

#define dataPrintf(...) setvbuf(stdout, NULL, _IONBF, 0), printf(__VA_ARGS__)

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

void dataDel(struct Data *data)
{
	for ( ; data->recordsQ > -1; --data->recordsQ) {
		for (size_t iV = data->records->valuesQ; iV > -1; --iV)
			free(data->records[data->recordsQ].values[iV].value);
		free(data->records[data->recordsQ].values);
	}
	free(data->records);
	for ( ; data->keysQ > -1; --data->keysQ)
		free(data->keys[data->keysQ].key);
	free(data->keys);
	perror("");
}

int dataNew(struct Data *data, size_t keysQ, size_t recordsQ, size_t valuesQ)
{
	data->keysQ = 0;
	data->recordsQ = 0;
	if (unlikely(!(data->records = malloc(recordsQ * sizeof(struct Record)))))
		goto ERROR;
	data->records->valuesQ = 0;
	if (unlikely(!(data->keys = malloc(keysQ * sizeof(struct Key)))))
		goto ERROR_FREE;
	for ( ; data->keysQ < keysQ; ++data->keysQ) {
		if (unlikely(!(data->keys[data->keysQ].key = malloc(MIN_STR_LEN))))
			goto ERROR_FREE;
		data->keys[data->keysQ].keySize = MIN_STR_LEN;
		data->keys[data->keysQ].keyLen = 0;
	}
	for ( ; data->recordsQ < recordsQ; ++data->recordsQ) {
		if (unlikely(!(data->records[data->recordsQ].values = malloc(valuesQ * sizeof(struct Value)))))
			goto ERROR_FREE;
		data->records[data->recordsQ].valuesQ = valuesQ;
		for ( ; data->records->valuesQ < valuesQ; ++data->records->valuesQ) {
			if (unlikely(!(data->records[data->recordsQ].values[data->records->valuesQ].value = malloc(MIN_STR_LEN))))
				goto ERROR_FREE;
			data->records[data->recordsQ].values[data->records->valuesQ].valueSize = MIN_STR_LEN;
			data->records[data->recordsQ].values[data->records->valuesQ].valueLen = 0;
		}
	}
	return 1;

ERROR_FREE:
	dataDel(data);
ERROR:
	perror("");
	return 0;
}

char *strtok_r(char *restrict s, const char *restrict delim, char **restrict save_ptr);

static inline size_t dataDelim(char *src, char delim)
{
	switch (delim) {
	case '|':
		return nixWcWordTilNlPipe(src);
	case ',':
		return nixWcWordTilNlComma(src);
	case '.':
		return nixWcWordTilNlDot(src);
	case '"':
		return nixWcWordTilNlDoubleQuote(src);
	case '\'':
		return nixWcWordTilNlQuote(src);
	case '\t':
		return nixWcWordTilNlTab(src);
	}
	return 0;
}

int dataLoad(struct Data *data, char src[], char delim)
{
	size_t lines = nixWcNl(src);
	data->recordsQ = lines;
	size_t values = nixWcWordTilNlPipe(src);
	data->keysQ = values;
	dataNew(data, values, lines, values);
	char *savePtr = src;
	char delimNl[] = {delim, '\n'};
	for (size_t iV = 0; iV < values; ++iV)
		data->keys[iV].key = strtok_r(savePtr, delimNl, &savePtr);
	for (size_t iL = 1; iL < lines; ++iL)
		for (size_t iV = 0; iV < values; ++iV)
			data->records[iL].values[iV].value = strtok_r(savePtr, delimNl, &savePtr);
	return 1;
}

static inline void dataPrintAll(struct Data *data)
{
	size_t values = data->keysQ;
	for (size_t iV = 0; iV < values; ++iV)
		dataPrintf("%s ", data->keys[iV].key);
	for (size_t iL = 1, lines = data->recordsQ; iL < lines; ++iL)
		for (size_t iV = 0; iV < values; ++iV)
			dataPrintf("%s\n", data->records[iL].values[iV].value);
}

static inline void dataPrintKeys(struct Data *data)
{
	for (size_t iV = 0, values = data->keysQ; iV < values; ++iV)
		dataPrintf("%s ", data->keys[iV].key);
}

static inline void dataPrintRecords(struct Data *data)
{
	for (size_t iL = 1, lines = data->recordsQ, values = data->keysQ; iL < lines; ++iL)
		for (size_t iV = 0; iV < values; ++iV)
			dataPrintf("%s ", data->records[iL].values[iV].value);
}

int main()
{
	struct Data data;
	size_t fileSize = nixSizeOfFile(FILENAME);
	char buf[fileSize];
	nixCat(FILENAME, fileSize, buf);
	dataLoad(&data, buf, '|');

	/* dataPrintAll(&data); */
	dataPrintKeys(&data);
	/* dataPrintRecords(&data); */
}
