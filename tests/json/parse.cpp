#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

#include <assert.h>
#include <stdio.h>

#include "../../src/json.h"

using namespace solana;
using namespace solana::json;

TEST_CASE("Parse Null") {
  json_value_t value;
  assert(json::parse("null", 4, &value) == JSON_PARSE_OK);
  assert(value.type == JSON_NULL);
  free_json(&value);
}

TEST_CASE("Parse True") {
  json_value_t value;
  assert(json::parse("true", 4, &value) == JSON_PARSE_OK);
  assert(value.type == JSON_TRUE);
  free_json(&value);
}

TEST_CASE("Parse False") {
  json_value_t value;
  assert(json::parse("false", 5, &value) == JSON_PARSE_OK);
  assert(value.type == JSON_FALSE);
  free_json(&value);
}

TEST_CASE("Parse Number") {
  json_value_t value;
  assert(json::parse("123", 3, &value) == JSON_PARSE_OK);
  assert(value.type == JSON_NUMBER);
  assert(value.number == 123);
  free_json(&value);
}

TEST_CASE("Parse String") {
  json_value_t value;
  assert(json::parse("\"hello\"", 7, &value) == JSON_PARSE_OK);
  assert(value.type == JSON_STRING);
  assert(strcmp(value.string, "hello") == 0);
  free_json(&value);
}

TEST_CASE("Parse Array") {
  json_value_t value;
  assert(json::parse("[1, 2, 3]", 9, &value) == JSON_PARSE_OK);
  assert(value.type == JSON_ARRAY);
  assert(value.array.size == 3);
  json_value_t *item = value.array.items;
  assert(item->number == 1);
  item = item->next;
  assert(item->number == 2);
  item = item->next;
  assert(item->number == 3);
  free_json(&value);
}

TEST_CASE("Parse Empty Object") {
  json_value_t value;
  assert(json::parse("{ }", 3, &value) == JSON_PARSE_OK);
  assert(value.type == JSON_OBJECT);
  assert(value.object.size == 0);
  free_json(&value);
}

TEST_CASE("Parse Object") {
  json_value_t value;
  assert(json::parse("{\"a\": 1, \"b\": 2}", 16, &value) == JSON_PARSE_OK);
  assert(value.type == JSON_OBJECT);
  assert(value.object.size == 2);
  json_value_t *item = value.object.items;
  assert(strcmp(item->key, "a") == 0);
  assert(item->number == 1);
  item = item->next;
  assert(strcmp(item->key, "b") == 0);
  assert(item->number == 2);
  free_json(&value);
}
