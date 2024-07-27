#pragma once
#include "includes.h"
#include "data_structures.h"
#include "dynamic_array.h"


void free_string(string_ptr_t array);

void print_str_array(string_ptr_t *array, size_t n);

int command_tokenize(char *input, string_tokens_t **str_ptr_array);
