#pragma once
#include "includes.h"
#include "data_structures.h"

void free_string_array(string_ptr_t *array, size_t n);

void print_str_array(string_ptr_t *array, size_t n);

int parse_command(char *input, string_ptr_t **str_array, size_t *n);
