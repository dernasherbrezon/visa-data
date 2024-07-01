#include "parse_args.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void usage() {
  printf("Usage:\n");
  printf("  -h print help\n");
  printf("  -r resource string for the device\n");
  printf("  -c comma separated list of channels to save\n");
  printf("  -t timeout in millis (default: 5000)\n");
  printf("  -f file prefix (optional)\n");
}

int parse_args(int argc, char **argv, config *result) {
  memset(result, 0, sizeof(config));
  int dopt;
  while ((dopt = getopt(argc, argv, "hr:tc:f")) != EOF) {
    switch (dopt) {
      case 'h':
        usage();
        return 1;
      case 'r':
        result->resource = optarg;
        break;
      case 't':
        result->timeout_ms = (uint32_t) strtol(optarg, NULL, 10);
        break;
      case 'c':
        for (int i = 0, j = 0; i < strlen(optarg); i++) {
          if (optarg[i] == ' ' || optarg[i] == ',') {
            continue;
          }
          int cur_channel = optarg[i] - '0';
          if (cur_channel == 0) {
            fprintf(stderr, "invalid channel id: %d\n", cur_channel);
            return 1;
          }
          result->channels[j] = cur_channel;
          j++;
        }
        break;
      case 'f':
        if (optarg == NULL) {
          fprintf(stderr, "file prefix specified but empty\n");
          return 1;
        }
        result->file_prefix = optarg;
        break;
      default:
        return 1;
    }
  }
  if (result->timeout_ms == 0) {
    result->timeout_ms = 5000;
  }
  if (result->resource == NULL) {
    fprintf(stderr, "resource is not specified\n");
    return 1;
  }
  if (result->channels[0] == 0) {
    fprintf(stderr, "channels are not specified\n");
    return 1;
  }
  return 0;
}