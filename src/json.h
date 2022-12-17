#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace solana {
namespace json {

#define JSON_PARSE_OK     0
#define JSON_PARSE_ERROR -1

#define JSON_OBJECT 1
#define JSON_ARRAY  2
#define JSON_STRING 3
#define JSON_NUMBER 4
#define JSON_TRUE   5
#define JSON_FALSE  6
#define JSON_NULL   7
/*
typedef enum {
  JSON_NULL,
  JSON_TRUE,
  JSON_FALSE,
  JSON_NUMBER,
  JSON_STRING,
  JSON_ARRAY,
  JSON_OBJECT
} json_type_t;
*/

// Data structure for storing a JSON value
typedef struct json_value_t {
  char *key;               // Key for this value (only used for objects)
  int type;                // Type of this value (null, boolean, number, string, array, object)
  union {
    int boolean;           // Boolean value (0 or 1)
    double number;         // Number value
    char *string;          // String value
    struct {               // Array value
      int size;            // Size of the array
      //struct json_value_t *values; // Array of values
    } array;
    struct {               // Object value
      int size;            // Size of the object
      //struct json_value_t *values; // Array of key-value pairs
    } object;
  };
  struct json_value_t *next; // Next value in the linked list
} json_value_t;

typedef struct json_parser {
  const char *input;
  const int length;
  int index;
  char* error;
} json_parser;

static int parse_json_value(json_parser *parser, json_value_t *value);
static void skip_whitespace(json_parser *parser);

static int parse_json_null(json_parser *parser, json_value_t* value) {
  value->type = JSON_NULL;
  if (strncmp(parser->input + parser->index, "null", 4) == 0) {
    parser->index += 4;
    return JSON_PARSE_OK;
  }
  return JSON_PARSE_ERROR;
}

static int parse_json_true(json_parser *parser, json_value_t* value) {
  value->type = JSON_TRUE;
  if (strncmp(parser->input + parser->index, "true", 4) == 0) {
    parser->index += 4;
    return JSON_PARSE_OK;
  }
  return JSON_PARSE_ERROR;
}

static int parse_json_false(json_parser *parser, json_value_t* value) {
  value->type = JSON_FALSE;
  if (strncmp(parser->input + parser->index, "false", 5) == 0) {
    parser->index += 5;
    return JSON_PARSE_OK;
  }
  return JSON_PARSE_ERROR;
}

static int parse_json_number(json_parser *parser, json_value_t* value) {
  value->type = JSON_NUMBER;
  char *end;
  double output = strtod(parser->input + parser->index, &end);
  if (end == parser->input + parser->index) {
    parser->error = "Expected number at start of value";
    return JSON_PARSE_ERROR;
  }
  parser->index += end - (parser->input + parser->index);
  value->number = output;
  return JSON_PARSE_OK;
}

static char *parse_string(json_parser *parser) {
  int start = parser->index;
  if (parser->input[parser->index++] != '"') {
    parser->error = "Expected '\"' at start of string";
    return NULL;
  }
  while (parser->input[parser->index] != '"') {
    if (parser->input[parser->index] == '\\') {
      parser->index++;
    }
    if (parser->input[parser->index] == '\0') {
      parser->error = "Expected '\"' at end of string";
      return NULL;
    }
    parser->index++;
  }
  int length = parser->index - start - 1;
  char *output = (char*)malloc(length + 1);
  memcpy(output, parser->input + start + 1, length);
  output[length] = '\0';
  parser->index++;
  return output;
}

static int parse_json_string(json_parser *parser, json_value_t* value) {
  value->type = JSON_STRING;
  value->string = parse_string(parser);
  return JSON_PARSE_OK;
}

static int parse_json_array(json_parser *parser, json_value_t* value) {
  value->type = JSON_ARRAY;
  value->array.size = 0;
  parser->index++;
  skip_whitespace(parser);
  json_value_t* prev = value;
  while (parser->index < parser->length) {
    if (parser->input[parser->index] == ']') {
      // End of the object reached, break out of the loop
      return JSON_PARSE_OK;
    }
    value->object.size++;
    json_value_t *item = (json_value_t*)malloc(sizeof(json_value_t));
    prev->next = item;
    prev = item;
    parse_json_value(parser, item);
    skip_whitespace(parser);
    if (parser->input[parser->index] == ',') {
      parser->index++;
      skip_whitespace(parser);
    } else if (parser->input[parser->index] != ']') {
      return JSON_PARSE_ERROR;
    }
  }
  return JSON_PARSE_ERROR;
}

static int parse_json_object(json_parser *parser, json_value_t* value) {
  value->type = JSON_OBJECT;
  value->object.size = 0;
  parser->index++;
  skip_whitespace(parser);
  json_value_t* prev = value;
  while (parser->index < parser->length) {
    if (parser->input[parser->index] == '}') {
      // End of the object reached, break out of the loop
      return JSON_PARSE_OK;
    }
    value->object.size++;
    json_value_t *item = (json_value_t*)malloc(sizeof(json_value_t));
    prev->next = item;
    prev = item;
    item->key = parse_string(parser);
    skip_whitespace(parser);
    if (parser->input[parser->index] != ':') {
      return JSON_PARSE_ERROR;
    }
    parser->index++;
    parse_json_value(parser, item);
    skip_whitespace(parser);
    if (parser->input[parser->index] == ',') {
      parser->index++;
      skip_whitespace(parser);
    } else if (parser->input[parser->index] != '}') {
      return JSON_PARSE_ERROR;
    }
  }
  return JSON_PARSE_ERROR;
}

static int parse_json_value(json_parser *parser, json_value_t *value) {
  skip_whitespace(parser);
  switch (parser->input[parser->index]) {
    case 'n': return parse_json_null(parser, value);
    case 't': return parse_json_true(parser, value);
    case 'f': return parse_json_false(parser, value);
    case '"': return parse_json_string(parser, value);
    case '[': return parse_json_array(parser, value);
    case '{': return parse_json_object(parser, value);
    default: return parse_json_number(parser, value);
  }
}

static void skip_whitespace(json_parser *parser) {
  while (parser->input[parser->index] == ' ' || parser->input[parser->index] == '\t' || parser->input[parser->index] == '\r' || parser->input[parser->index] == '\n') {
    parser->index++;
  }
}

int parse(const char *json, const int length, json_value_t *value) {
  json_parser json_parser = { .input = json, .length = length, .index = 0, .error = NULL };
  return parse_json_value(&json_parser, value);
}

//TODO
//int stringify(json_value_t *value, char *json, int *length) {
//}

/*
    while (val) {
      printf("%s: ", val->key);
      switch (val->type) {
        case 0:  // Null
          printf("null");
          break;
        case 1:  // Boolean
          printf(val->boolean ? "true" : "false");
          break;
        case 2:  // Number
          printf("%.2f", val->number);
          break;
        case 3:  // String
          printf("%s", val->string);
          break;
        case 4:  // Array
          printf("[...]");
          break;
        case 5:  // Object
          printf("{...}");
          break;
      }
      printf("\n");
      val = val->next;
    }
*/

/*
free_json(value);
*/

}
}
