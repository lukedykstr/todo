/**
 * Generic vector library
 *
 * Proof of concept; not meant to be very robust.
 * Just make sure we have no memory leaks and that it works.
 * 
 * The vector will hold a dynamically sized memory area (basically an
 * array) of **addresses**.  Remember that all addresses are the same size.
 *
 * Create a new vector with the macros given in the .h file:
 * vector* nums = VECTOR_NEW(int, 100)	-> New vector of ints, initial size 100.
 * vector* char = VECTOR_NEW(char, 10)  -> New vector of char, initial size 10.
 * etc.
 *
 * Add to a vector by passing the address of the element you wish to add:
 * int x = 42;
 * vec_add(nums, &x);
 *
 * Get an element from the vector with the provided macro:
 * int y = VECTOR_GET(nums, 0)  -> Get element #0 from the vector.
 *
 * January 2024
 * Ira Woodring
 **/

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "vector.h"

static bool vec_grow(vector* vec);
static bool vec_shrink(vector* vec);

/**
 * Make a new vector.  This function acts like a constructor
 * (or it would if C had objects).  Set default state
 * of the struct after you make memory for it,
 * and return the created vector.  Default state is length
 * of zero, capacity of initial_size, type_size of type_size
 * and filled percentage 0.
 */

vector* new_vec(size_t type_size, size_t initial_size){
	vector* vec = malloc(sizeof(vector));
	vec -> length = 0;
	vec -> capacity = initial_size;
	vec -> type_size = type_size;
	vec -> filled_percentage = 0;
	vec -> data = malloc(sizeof(void*) * initial_size);

	return vec;
}

/**
 * Add an item to the vector.
 * We are given an address for the element.  We need to:
 * - see if we have space in our address table for another element
 * - grow if we don't
 * - create new area in memory of the size of the element we are given
 * - use memcpy to copy the memory from the address of the element to the new area
 * - store the pointer given back in the vector at the current length
 * - increase length
 */

void vec_add(vector* vec, void* element){
	if(vec -> length == vec -> capacity) {
		vec_grow(vec);
	}
	
	// allocate new space for copy of element
	void* elcopy = malloc(vec -> type_size);
	// copy element to new space
	memcpy(elcopy, element, vec -> type_size);
	// store pointer to copy at the end of vector.data
	// end of data = start + (size of void*) * current length
	(vec -> data)[vec -> length] = elcopy;
	// increase length
	vec -> length ++;
}

/**
 * Get an item from the vector, at the given index (it is exists).
 * No tricks here; just make sure index is in correct bounds,
 * then return the address of the element if it is.
 * Return NULL otherwise.
 */

void* vec_get(vector* vec, size_t index){
	if(index < (vec -> length)) {
		return (vec -> data)[index];
	}

	return NULL;
}

/**
 * Remove an item from the vector, and mark its memory
 * area NULL.
 *
 * Extra credit: If we go below 25% usage, resize the vector
 * to half its current size by calling vec_shrink.
 */

void* vec_remove(vector* vec, size_t index){
	// shift all elements right of index left 1
	// start at the index one after
	index ++;

	while(index < (vec -> length)) {
		(vec -> data)[index - 1] = (vec -> data)[index];
		((vec -> data)[index]) = NULL;
		index ++;
	}

	// update length
	(vec -> length) --;

	// if below 25% usage, shrink vector
	// usage = length / capacity
	// must convert to float otherwise it will use integer division, which will always
	// result in 0
	if((((float)(vec -> length) / (float)(vec -> capacity)) < 0.25)) {
		vec_shrink(vec);
	}
}

/**
 * Cleanup our memory when the user is done with the vector.
 * Keep in mind we allocated memory for the vector itself,
 * the dynamic array called data, and each of its elements.
 */

void vec_cleanup(vector* vec){
	// Remember; length is the current location
	// to store an element, so don't go past it.
	
	// Free each element*, starting at data[0] and ending at data[length - 1]
	for(int i = 0; i < vec -> length; i ++) {
		free((vec -> data)[i]);
	}

	free(vec -> data);
	free(vec);
}

/**
 * Shrink the vector if it is largely empty.
 * Used if we remove a lot of items, to free up memory.
 *
 * Extra Credit.
 **/

static bool vec_shrink(vector* vec){
	// halve the capacity, malloc new space for data
	(vec -> capacity) /= 2;
	void** data_copy = malloc(sizeof(void*) * (vec -> capacity));
	// copy data to data_copy
	memcpy(data_copy, vec -> data, sizeof(void*) * (vec -> length));
	// free old data, set data to data_copy. It is now half the size, with the same data
	free(vec -> data);
	(vec -> data) = data_copy;

	return false;
}

/**
 * Grow the vector if we run out of room.
 *
 * NOTE: Do not use realloc; I want to see that you understand how
 * to do it on your own.
 *
 * Note: "static" in this context makes this function invisible outside
 * of this file.
 */

static bool vec_grow(vector* vec){
	// double the capacity, malloc new space for data
	(vec -> capacity) *= 2;
	void** data_copy = malloc(sizeof(void*) * (vec -> capacity));
	// copy data to data_copy
	memcpy(data_copy, vec -> data, sizeof(void*) * (vec -> length));
	// free old data, set data to data_copy. It is now twice the size, with the same data
	free(vec -> data);
	(vec -> data) = data_copy;
}

