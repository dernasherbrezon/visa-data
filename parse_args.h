#ifndef VISA_DATA_PARSE_ARGS_H
#define VISA_DATA_PARSE_ARGS_H

#include <visa.h>

#define MAX_NUMBER_OF_CHANNELS 10

typedef struct {
  ViChar *resource;
  ViUInt32 timeout_ms;
  int channels[MAX_NUMBER_OF_CHANNELS];
  char *file_prefix;
} config;

int parse_args(int argc, char **argv, config *config);

#endif //VISA_DATA_PARSE_ARGS_H
