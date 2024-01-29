/**
 * Generic vector library
 *
 * Proof of concept; not very robust.
 * January 2024
 * Ira Woodring
 **/



#ifndef		__H__VECTOR__
#define		__H__VECTOR__

#include <stdlib.h>

#define NEW_VECTOR(type, size)			new_vec(sizeof(type), size)
#define VECTOR_GET(type, vec, index)		*(type*)vec_get(vec, index)

// Data structure vor a vector.  Length is how many pieces
// of data are currently held.  Capacity is the total we can
// hold.  type_size is the number of bytes per pieces of data.
typedef struct vector_t {
	size_t length;
	size_t capacity;
	size_t type_size;
	float filled_percentage;
	void** data;
} vector;

// Constructs and returns a new vector of the specified type
// with the given initial size.
vector* new_vec(size_t, size_t);
int vec_length(vector*);
void vec_add(vector*, void* element);
void* vec_get(vector*, size_t index);
void* vec_remove(vector*, size_t index);
void vec_cleanup(vector*);

#endif
