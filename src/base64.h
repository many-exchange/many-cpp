#pragma once

#include <assert.h>
#include <stdlib.h>

namespace solana {
namespace base64 {

  static const char base64_encoding_table[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/' };

  static int mod_table[] = {0, 2, 1};

  int encode(const char *input, const int input_length, char *output, const int max_output_length) {
    int output_length = 4 * ((input_length + 2) / 3);
    assert(output_length <= max_output_length);

    for (int i = 0, j = 0; i < input_length;) {
      int octet_a = i < input_length ? input[i++] : 0;
      int octet_b = i < input_length ? input[i++] : 0;
      int octet_c = i < input_length ? input[i++] : 0;

      int triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

      output[j++] = base64_encoding_table[(triple >> 3 * 6) & 0x3F];
      output[j++] = base64_encoding_table[(triple >> 2 * 6) & 0x3F];
      output[j++] = base64_encoding_table[(triple >> 1 * 6) & 0x3F];
      output[j++] = base64_encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (int i = 0; i < mod_table[input_length % 3]; i++)
        output[output_length - 1 - i] = '=';

    return output_length;
  }

  static const char base64_decoding_table[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 62, 0, 0, 0, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,
    0, 0, 0, -1, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
    14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 0, 0, 0, 0, 0, 26,
    27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44,
    45, 46, 47, 48, 49, 50, 51, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };

  int decode(const char *input, const int input_length, char *output, const int max_output_length) {
    assert(input_length % 4 == 0);

    int output_length = input_length / 4 * 3;
    assert(output_length <= max_output_length);
    if (input[input_length - 1] == '=') (output_length)--;
    if (input[input_length - 2] == '=') (output_length)--;

    for (int i = 0, j = 0; i < input_length;) {
      int sextet_a = input[i] == '=' ? 0 & i++ : base64_decoding_table[(int)input[i++]];
      int sextet_b = input[i] == '=' ? 0 & i++ : base64_decoding_table[(int)input[i++]];
      int sextet_c = input[i] == '=' ? 0 & i++ : base64_decoding_table[(int)input[i++]];
      int sextet_d = input[i] == '=' ? 0 & i++ : base64_decoding_table[(int)input[i++]];

      int triple = (sextet_a << 3 * 6)
                    + (sextet_b << 2 * 6)
                    + (sextet_c << 1 * 6)
                    + (sextet_d << 0 * 6);

      if (j < output_length) output[j++] = (triple >> 2 * 8) & 0xFF;
      if (j < output_length) output[j++] = (triple >> 1 * 8) & 0xFF;
      if (j < output_length) output[j++] = (triple >> 0 * 8) & 0xFF;
    }

    return output_length;
  }

}
}
