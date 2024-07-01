#include <visa.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "parse_args.h"

#define MAX_BUFFER_LENGTH 1000

ViSession resource_manager;
ViSession instrument;
ViChar buffer[MAX_BUFFER_LENGTH];

typedef struct {
  uint8_t format;
  uint8_t type;
  uint32_t points;
  uint32_t count;
  float x_increment;
  uint32_t x_origin;
  uint32_t x_reference;
  float y_increment;
  uint32_t y_origin;
  uint32_t y_reference;
} preamble;

#define ERROR_CHECK(y, x)           \
  do {                           \
    ViStatus __err_rc = (x);          \
    if (__err_rc < VI_SUCCESS) {   \
      memset(buffer, 0, sizeof(buffer));                              \
      viStatusDesc(resource_manager, __err_rc, buffer);                             \
      fprintf(stderr, "%s: %s\n", y, buffer);                             \
      return __err_rc;           \
    }                            \
  } while (0)

int write_command(const char *command) {
  ViUInt32 io_bytes;
  size_t sent = 0;
  size_t total = strlen(command);
  while (sent < total) {
    size_t remaining = total - sent;
    ViStatus code = viWrite(instrument, (ViConstBuf) (command + sent), (ViUInt32) remaining, &io_bytes);
    if (code < VI_SUCCESS) {
      return code;
    }
    sent += io_bytes;
  }
  return 0;
}

int query_preamble(preamble *result) {
  ViStatus code = write_command(":WAVeform:PREamble?");
  if (code < VI_SUCCESS) {
    return code;
  }
  code = viScanf(instrument, "%d,%d," PRIu32 "," PRIu32 ",%f," PRIu32 "," PRIu32 ",%f," PRIu32 "," PRIu32, result->format, result->type, result->points, result->count, result->x_increment, result->x_origin, result->x_reference, result->y_increment, result->y_origin, result->y_reference);
  return code;
}

int query_header(uint32_t *result) {
  char tmc_header[12];
  memset(tmc_header, 0, sizeof(tmc_header));
  size_t total = sizeof(tmc_header) - 1; // 1 is for '\0'
  size_t current = 0;
  ViUInt32 io_bytes;
  while (current < total) {
    size_t remaining = total - current;
    ViStatus code = viRead(instrument, (ViByte *) (buffer + current), remaining, &io_bytes);
    if (code < VI_SUCCESS) {
      return code;
    }
    current += io_bytes;
  }
  *result = (uint32_t) strtol(buffer + 2, NULL, 10); //skip #9 at the beginning
  return 0;
}

int main(int argc, char **argv) {

  config config;
  int code = parse_args(argc, argv, &config);
  if (code != 0) {
    return code;
  }
  ViAccessMode access_mode = VI_NULL;
  // Get VISA resource manager
  ViStatus status;
  status = viOpenDefaultRM(&resource_manager);
  if (status < VI_SUCCESS) {
    printf("Could not open VISA resource manager.\n");
    return 0;
  }

  // Connect to instrument
  ERROR_CHECK("unable to connect to instrument", viOpen(resource_manager, config.resource, access_mode, config.timeout_ms, &instrument));
  ERROR_CHECK("unable to set timeout", viSetAttribute(instrument, VI_ATTR_TMO_VALUE, config.timeout_ms));
  // Set term char
  // viSetAttribute(instrument, VI_ATTR_TERMCHAR_EN, '\n');
  ERROR_CHECK("unable to set mode", write_command(":WAV:MODE NORM"));
  ERROR_CHECK("unable to set form", write_command(":WAV:FORM BYTE"));
  preamble screen_stats;
  ERROR_CHECK("unable to query preamble", query_preamble(&screen_stats));

  for (int i = 0; i < MAX_NUMBER_OF_CHANNELS; i++) {
    if (config.channels[i] == 0) {
      break;
    }
    printf("reading channel %d\n", config.channels[i]);
    snprintf(buffer, MAX_BUFFER_LENGTH, ":WAV:SOUR CHAN%d", config.channels[i]);
    ERROR_CHECK("unable to set channel", write_command(buffer));
    ERROR_CHECK("unable to request data", write_command(":WAV:DATA?"));
    uint32_t number_of_points;
    ERROR_CHECK("unable to read header", query_header(&number_of_points));
    if (config.file_prefix != NULL) {
      snprintf(buffer, MAX_BUFFER_LENGTH, "%s_%d.csv", config.file_prefix, config.channels[i]);
    } else {
      snprintf(buffer, MAX_BUFFER_LENGTH, "%d.csv", config.channels[i]);
    }
    FILE *fp = fopen(buffer, "w");
    if (fp == NULL) {
      fprintf(stderr, "unable to open file: %s\n", buffer);
      return 1;
    }
    //FIXME read actual data and write to file
  }
  printf("done");
  return 0;
}
