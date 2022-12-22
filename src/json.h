#pragma once

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace solana {
namespace json {

#define JSON_PARSE_OK     0
#define JSON_PARSE_ERROR -1

#define JSON_STRINGIFY_OK     0
#define JSON_STRINGIFY_ERROR -1

#define JSON_NULL   1
#define JSON_TRUE   2
#define JSON_FALSE  3
#define JSON_NUMBER 4
#define JSON_STRING 5
#define JSON_ARRAY  6
#define JSON_OBJECT 7

// Data structure for storing a JSON value
typedef struct json_value_t {
  const char *key;   // Key for this value (only used for objects)
  int type;          // Type of this value (null, true, false, number, string, array, object)
  union {
    uint64_t number; // Number value
    struct {
      int length;
      const char *value;
    } string;
    struct {         // Array value
      int size;      // Size of the array
      struct json_value_t *items;
    } array;
    struct {         // Object value
      int size;      // Size of the object
      struct json_value_t *items;
    } object;
  };
  struct json_value_t *next; // Next value in the linked list

  json_value_t* operator[](int index) {
    if (type == JSON_ARRAY) {
      json_value_t *item = array.items;
      for (int i = 0; i < index; i++) {
        item = item->next;
      }
      return item;
    }
    return nullptr;
  }

  json_value_t* operator[](const char* key) {
    if (type == JSON_OBJECT) {
      json_value_t *item = object.items;
      for (int i = 0; i < object.size; i++) {
        if (item->key == key) {
          return item;
        }
        item = item->next;
      }
    }
    return nullptr;
  }

} json_value_t;

void free_json(json_value_t *value) {
  if (value->type == JSON_ARRAY) {
    json_value_t *item = value->array.items;
    for (int i = 0; i < value->array.size; i++) {
      json_value_t *next = item->next;
      free_json(item);
      item = next;
    }
  } else if (value->type == JSON_OBJECT) {
    json_value_t *item = value->object.items;
    for (int i = 0; i < value->object.size; i++) {
      json_value_t *next = item->next;
      free_json(item);
      item = next;
    }
  }
}



struct json_array_t;
struct json_object_t;

struct json_array_t {
  json_value_t head;
  json_value_t *tail;

  json_array_t() : head() {
    head.type = JSON_ARRAY;
    head.array.size = 0;
    head.array.items = NULL;
    tail = &head;
  }

  void add(uint64_t number) {
    json_value_t *item = (json_value_t*)malloc(sizeof(json_value_t));
    item->type = JSON_NUMBER;
    item->number = number;
    add(item);
  }

  void add(const char* string) {
    json_value_t *item = (json_value_t*)malloc(sizeof(json_value_t));
    item->type = JSON_STRING;
    item->string.length = strlen(string);
    item->string.value = string;
    add(item);
  }

  void add(json_array_t& array);

  void add(json_object_t& object);

  void add(json_value_t *item) {
    if (head.array.size == 0) {
      head.array.items = item;
    } else {
      tail->next = item;
    }
    head.array.size++;

    item->next = NULL;
    tail = item;
  }

};

struct json_object_t {
  json_value_t head;
  json_value_t *tail;

  json_object_t() : head() {
    head.type = JSON_OBJECT;
    head.object.size = 0;
    head.object.items = NULL;
    tail = &head;
  }

  void add(const char* key, uint64_t number) {
    json_value_t *item = (json_value_t*)malloc(sizeof(json_value_t));
    item->type = JSON_NUMBER;
    item->number = number;
    add(key, item);
  }

  void add(const char* key, const char* string) {
    json_value_t *item = (json_value_t*)malloc(sizeof(json_value_t));
    item->type = JSON_STRING;
    item->string.length = strlen(string);
    item->string.value = string;
    add(key, item);
  }

  void add(const char* key, json_array_t& array);

  void add(const char* key, json_object_t& object);

  void add(const char* key, json_value_t *item) {
    if (head.object.size == 0) {
      head.object.items = item;
    } else {
      tail->next = item;
    }
    head.object.size++;

    item->key = key;
    item->next = NULL;
    tail = item;
  }

};

void json_array_t::add(json_array_t& array) {
  add(&array.head);
}

void json_array_t::add(json_object_t& object) {
  add(&object.head);
}

void json_object_t::add(const char* key, json_array_t& array) {
  add(key, &array.head);
}

void json_object_t::add(const char* key, json_object_t& object) {
  add(key, &object.head);
}



typedef struct json_parser {
  const char *input;
  const int length;
  int index;
  const char* error;
} json_parser;

static int parse_json_value(json_parser *parser, json_value_t *value);
static void skip_whitespace(json_parser *parser);

static int parse_json_null(json_parser *parser, json_value_t* value) {
  value->key = NULL;
  value->type = JSON_NULL;
  value->next = NULL;
  if (strncmp(parser->input + parser->index, "null", 4) == 0) {
    parser->index += 4;
    return JSON_PARSE_OK;
  }
  return JSON_PARSE_ERROR;
}

static int parse_json_true(json_parser *parser, json_value_t* value) {
  value->key = NULL;
  value->type = JSON_TRUE;
  value->next = NULL;
  if (strncmp(parser->input + parser->index, "true", 4) == 0) {
    parser->index += 4;
    return JSON_PARSE_OK;
  }
  return JSON_PARSE_ERROR;
}

static int parse_json_false(json_parser *parser, json_value_t* value) {
  value->key = NULL;
  value->type = JSON_FALSE;
  value->next = NULL;
  if (strncmp(parser->input + parser->index, "false", 5) == 0) {
    parser->index += 5;
    return JSON_PARSE_OK;
  }
  return JSON_PARSE_ERROR;
}

static int parse_json_number(json_parser *parser, json_value_t* value) {
  value->key = NULL;
  value->type = JSON_NUMBER;
  value->next = NULL;
  char *end;
  uint64_t output = strtoul(parser->input + parser->index, &end, 10);
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
  value->key = NULL;
  value->type = JSON_STRING;
  value->next = NULL;
  value->string.value = parse_string(parser);
  value->string.length = strlen(value->string.value);
  return JSON_PARSE_OK;
}

static int parse_json_array(json_parser *parser, json_value_t* value) {
  value->key = NULL;
  value->type = JSON_ARRAY;
  value->next = NULL;
  value->array.size = 0;
  value->array.items = NULL;
  parser->index++;
  skip_whitespace(parser);
  json_value_t* prev = NULL;
  while (parser->index < parser->length) {
    if (parser->input[parser->index] == ']') {
      parser->index++;
      // End of the object reached, break out of the loop
      return JSON_PARSE_OK;
    }
    json_value_t *item = (json_value_t*)malloc(sizeof(json_value_t));
    if (parse_json_value(parser, item) != JSON_PARSE_OK) {
      return JSON_PARSE_ERROR;
    }

    if (value->array.size == 0) {
      value->array.items = item;
      prev = item;
    } else {
      prev->next = item;
      prev = item;
    }
    value->array.size++;

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
  value->key = NULL;
  value->type = JSON_OBJECT;
  value->next = NULL;
  value->object.size = 0;
  value->object.items = NULL;
  parser->index++;
  skip_whitespace(parser);
  json_value_t* prev = NULL;
  while (parser->index < parser->length) {
    if (parser->input[parser->index] == '}') {
      parser->index++;
      // End of the object reached, break out of the loop
      return JSON_PARSE_OK;
    }

    json_value_t *item = (json_value_t*)malloc(sizeof(json_value_t));
    item->key = parse_string(parser);
    skip_whitespace(parser);
    if (parser->input[parser->index] != ':') {
      return JSON_PARSE_ERROR;
    }
    parser->index++;
    skip_whitespace(parser);
    if (parse_json_value(parser, item) != JSON_PARSE_OK) {
      return JSON_PARSE_ERROR;
    }


    if (value->object.size == 0) {
      value->object.items = item;
      prev = item;
    } else {
      prev->next = item;
      prev = item;
    }
    value->object.size++;


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

bool parse(std::string json, json_value_t& value) {
  return parse(json.c_str(), json.length(), &value);
}



void print(json_value_t *value) {
  switch (value->type)
  {
    case JSON_NULL:
      printf("null");
      break;
    case JSON_TRUE:
      printf("true");
      break;
    case JSON_FALSE:
      printf("false");
      break;
    case JSON_NUMBER:
      printf("%" PRIu64, value->number);
      break;
    case JSON_STRING:
      printf("\"%s\"", value->string.value);
      break;
    case JSON_ARRAY:
    {
      printf("[");
      json_value_t *item = value->array.items;
      for (int i = 0; i < value->array.size; i++) {
        if (i > 0) {
          printf(", ");
        }
        print(item);
        item = item->next;
      }
      assert(item == NULL);
      printf("]");
      break;
    }
    case JSON_OBJECT:
    {
      printf("{");
      json_value_t *item = value->object.items;
      for (int i = 0; i < value->object.size; i++) {
        if (i > 0) {
          printf(", ");
        }
        printf("\"%s\"", item->key);
        printf(": ");
        print(item);
        item = item->next;
      }
      assert(item == NULL);
      printf("}");
      break;
    }
    default:
      throw "Invalid json type.";
  }
}



static int stringify_json(json_value_t *value, char *buffer, int *offset, int *length) {
  switch (value->type)
  {
    case JSON_NULL:
    {
      strncpy(&buffer[*offset], "null", 4);
      (*offset) += 4;
      (*length) += 4;
      return JSON_STRINGIFY_OK;
    }
    case JSON_TRUE:
    {
      strncpy(&buffer[*offset], "true", 4);
      (*offset) += 4;
      (*length) += 4;
      return JSON_STRINGIFY_OK;
    }
    case JSON_FALSE:
    {
      strncpy(&buffer[*offset], "false", 5);
      (*offset) += 5;
      (*length) += 5;
      return JSON_STRINGIFY_OK;
    }
    case JSON_NUMBER:
    {
      int len = sprintf(&buffer[*offset], "%" PRIu64, value->number);
      (*offset) += len;
      (*length) += len;
      return JSON_STRINGIFY_OK;
    }
    case JSON_STRING:
    {
      int len = sprintf(&buffer[*offset], "\"%s\"", value->string.value);
      (*offset) += len;
      (*length) += len;
      return JSON_STRINGIFY_OK;
    }
    case JSON_ARRAY:
    {
      sprintf(&buffer[*offset], "[");
      (*offset)++;
      (*length)++;
      json_value_t *item = value->array.items;
      for (int i = 0; i < value->array.size; i++) {
        if (i > 0) {
          sprintf(&buffer[*offset], ",");
          (*offset)++;
          (*length)++;
        }
        stringify_json(item, buffer, offset, length);
        item = item->next;
      }
      assert(item == NULL);
      sprintf(&buffer[*offset], "]");
      (*offset)++;
      (*length)++;
      return JSON_STRINGIFY_OK;
    }
    case JSON_OBJECT:
    {
      sprintf(&buffer[*offset], "{");
      (*offset)++;
      (*length)++;
      json_value_t *item = value->object.items;
      for (int i = 0; i < value->object.size; i++) {
        if (i > 0) {
          sprintf(&buffer[*offset], ",");
          (*offset)++;
          (*length)++;
        }
        int len = sprintf(&buffer[*offset],"\"%s\"", item->key);
        (*offset) += len;
        (*length) += len;
        sprintf(&buffer[*offset], ":");
        (*offset)++;
        (*length)++;
        stringify_json(item, buffer, offset, length);
        item = item->next;
      }
      assert(item == NULL);
      sprintf(&buffer[*offset], "}");
      (*offset)++;
      (*length)++;
      return JSON_STRINGIFY_OK;
    }
    default:
    {
      return JSON_STRINGIFY_ERROR;
    }
  }
}

int stringify(json_value_t *value, char *buffer, int *length) {
  int offset = 0;
  return stringify_json(value, buffer, &offset, length);
}

}
}
