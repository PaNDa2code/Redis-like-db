#pragma once
#include "data_structures.h"

void expiry_cleanup_exit();

void expiry_data_push(string_container_t* value);

void cancel_container_timer(string_container_t *container);

void expiry_cleanup();
