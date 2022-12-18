#pragma once

#include <stdio.h>
#include <assert.h>

#include "../../src/json.h"

using namespace solana;
using namespace solana::json;

TEST_CASE("Stringify Null") {
  json_value_t value;
  assert(json::parse("null", 4, &value) == JSON_PARSE_OK);

  char buffer[64];
  memset(buffer, 0, 1);
  int length = 0;
  assert(json::stringify(&value, buffer, &length) == JSON_STRINGIFY_OK);
  assert(length == 4);
  assert(strncmp(buffer, "null", 4) == 0);
}

TEST_CASE("Stringify True") {
  json_value_t value;
  assert(json::parse("true", 4, &value) == JSON_PARSE_OK);

  char buffer[64];
  memset(buffer, 0, 1);
  int length = 0;
  assert(json::stringify(&value, buffer, &length) == JSON_STRINGIFY_OK);
  assert(length == 4);
  assert(strncmp(buffer, "true", 4) == 0);
}

TEST_CASE("Stringify False") {
  json_value_t value;
  assert(json::parse("false", 5, &value) == JSON_PARSE_OK);

  char buffer[64];
  memset(buffer, 0, 1);
  int length = 0;
  assert(json::stringify(&value, buffer, &length) == JSON_STRINGIFY_OK);
  assert(length == 5);
  assert(strncmp(buffer, "false", 5) == 0);
}

TEST_CASE("Stringify Number") {
  json_value_t value;
  assert(json::parse("123", 3, &value) == JSON_PARSE_OK);

  char buffer[64];
  memset(buffer, 0, 1);
  int length = 0;
  assert(json::stringify(&value, buffer, &length) == JSON_STRINGIFY_OK);
  assert(length == 3);
  assert(strncmp(buffer, "123", 3) == 0);
}

TEST_CASE("Stringify String") {
  json_value_t value;
  assert(json::parse("\"hello\"", 7, &value) == JSON_PARSE_OK);

  char buffer[64];
  memset(buffer, 0, 1);
  int length = 0;
  assert(json::stringify(&value, buffer, &length) == JSON_STRINGIFY_OK);
  assert(length == 7);
  assert(strncmp(buffer, "\"hello\"", 7) == 0);
}

TEST_CASE("Stringify Array") {
  json_value_t value;
  assert(json::parse("[1, 2, 3]", 9, &value) == JSON_PARSE_OK);

  char buffer[64];
  memset(buffer, 0, 1);
  int length = 0;
  assert(json::stringify(&value, buffer, &length) == JSON_STRINGIFY_OK);
  assert(length == 9);
  assert(strncmp(buffer, "[1, 2, 3]", 9) == 0);
}

TEST_CASE("Stringify Empty Object") {
  json_value_t value;
  assert(json::parse("{ }", 3, &value) == JSON_PARSE_OK);

  char buffer[64];
  memset(buffer, 0, 1);
  int length = 0;
  assert(json::stringify(&value, buffer, &length) == JSON_STRINGIFY_OK);
  assert(length == 2);
  assert(strncmp(buffer, "{}", 2) == 0);
}

TEST_CASE("Stringify Object") {
  json_value_t value;
  assert(json::parse("{\"a\": 1, \"b\": 2}", 16, &value) == JSON_PARSE_OK);

  char buffer[64];
  memset(buffer, 0, 1);
  int length = 0;
  assert(json::stringify(&value, buffer, &length) == JSON_STRINGIFY_OK);
  assert(length == 16);
  assert(strncmp(buffer, "{\"a\": 1, \"b\": 2}", 16) == 0);
}
