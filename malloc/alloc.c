//used ChatGPT for initial design and debugging - it works well
/**
 * malloc
 * CS 341 - Fall 2023
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

// typedef struct _metadata_entry_t {
//     void *ptr;
//     size_t size;
//     int free;
//     struct _metadata_entry_t *next;
// } metadata_entry_t;

// static metadata_entry_t* head = NULL;

// /**
//  * Allocate space for array in memory
//  *
//  * Allocates a block of memory for an array of num elements, each of them size
//  * bytes long, and initializes all its bits to zero. The effective result is
//  * the allocation of an zero-initialized memory block of (num * size) bytes.
//  *
//  * @param num
//  *    Number of elements to be allocated.
//  * @param size
//  *    Size of elements.
//  *
//  * @return
//  *    A pointer to the memory block allocated by the function.
//  *
//  *    The type of this pointer is always void*, which can be cast to the
//  *    desired type of data pointer in order to be dereferenceable.
//  *
//  *    If the function failed to allocate the requested block of memory, a
//  *    NULL pointer is returned.
//  *
//  * @see http://www.cplusplus.com/reference/clibrary/cstdlib/calloc/
//  */
// void *calloc(size_t num, size_t size) {
//     size_t total_size = num * size;
//     void *ptr = malloc(total_size);
//     if (ptr != NULL) {
//         memset(ptr, 0, total_size);
//     }
//     return ptr;
// }

// /**
//  * Allocate memory block
//  *
//  * Allocates a block of size bytes of memory, returning a pointer to the
//  * beginning of the block.  The content of the newly allocated block of
//  * memory is not initialized, remaining with indeterminate values.
//  *
//  * @param size
//  *    Size of the memory block, in bytes.
//  *
//  * @return
//  *    On success, a pointer to the memory block allocated by the function.
//  *
//  *    The type of this pointer is always void*, which can be cast to the
//  *    desired type of data pointer in order to be dereferenceable.
//  *
//  *    If the function failed to allocate the requested block of memory,
//  *    a null pointer is returned.
//  *
//  * @see http://www.cplusplus.com/reference/clibrary/cstdlib/malloc/
//  */
// void *malloc(size_t size) {
//     // implement malloc!
//     metadata_entry_t *p = head;
//     metadata_entry_t *chosen = NULL;

//     while(p != NULL) {
//         if (p->free && p->size >= size) {
//             if (chosen == NULL || (chosen && p->size < chosen->size)) {
//                 chosen = p;
//             }
//         }
//         p = p->next;
//     }
//     if(chosen) {
//         chosen->free = 0;
//         return chosen->ptr;
//     }
//     chosen = sbrk(sizeof(metadata_entry_t));
//     chosen->ptr = sbrk(size);
//     if(chosen->ptr == (void*)-1) {
//         return NULL;
//     } 
//     chosen->size = size;
//     chosen->free = 0; 
//     chosen->next = head;
//     head = chosen;
//     return chosen->ptr;
// }

// /**
//  * Deallocate space in memory
//  *
//  * A block of memory previously allocated using a call to malloc(),
//  * calloc() or realloc() is deallocated, making it available again for
//  * further allocations.
//  *
//  * Notice that this function leaves the value of ptr unchanged, hence
//  * it still points to the same (now invalid) location, and not to the
//  * null pointer.
//  *
//  * @param ptr
//  *    Pointer to a memory block previously allocated with malloc(),
//  *    calloc() or realloc() to be deallocated.  If a null pointer is
//  *    passed as argument, no action occurs.
//  */
// void free(void *ptr) {
//     if(!ptr) return;
//     metadata_entry_t *p = head;
//     while (p) {
//         if(p->ptr == ptr) {
//             p->free = 1;
//             break;
//         }
//         p = p->next;
//     }
//     return;
// }

// /**
//  * Reallocate memory block
//  *
//  * The size of the memory block pointed to by the ptr parameter is changed
//  * to the size bytes, expanding or reducing the amount of memory available
//  * in the block.
//  *
//  * The function may move the memory block to a new location, in which case
//  * the new location is returned. The content of the memory block is preserved
//  * up to the lesser of the new and old sizes, even if the block is moved. If
//  * the new size is larger, the value of the newly allocated portion is
//  * indeterminate.
//  *
//  * In case that ptr is NULL, the function behaves exactly as malloc, assigning
//  * a new block of size bytes and returning a pointer to the beginning of it.
//  *
//  * In case that the size is 0, the memory previously allocated in ptr is
//  * deallocated as if a call to free was made, and a NULL pointer is returned.
//  *
//  * @param ptr
//  *    Pointer to a memory block previously allocated with malloc(), calloc()
//  *    or realloc() to be reallocated.
//  *
//  *    If this is NULL, a new block is allocated and a pointer to it is
//  *    returned by the function.
//  *
//  * @param size
//  *    New size for the memory block, in bytes.
//  *
//  *    If it is 0 and ptr points to an existing block of memory, the memory
//  *    block pointed by ptr is deallocated and a NULL pointer is returned.
//  *
//  * @return
//  *    A pointer to the reallocated memory block, which may be either the
//  *    same as the ptr argument or a new location.
//  *
//  *    The type of this pointer is void*, which can be cast to the desired
//  *    type of data pointer in order to be dereferenceable.
//  *
//  *    If the function failed to allocate the requested block of memory,
//  *    a NULL pointer is returned, and the memory block pointed to by
//  *    argument ptr is left unchanged.
//  *
//  * @see http://www.cplusplus.com/reference/clibrary/cstdlib/realloc/
//  */
// void *realloc(void *ptr, size_t size) {
//     if (ptr == NULL) { 
//         return malloc(size);
//         }
//     metadata_entry_t* entry = ((metadata_entry_t*)ptr) - 1;
//     assert (entry->ptr == ptr);
//     assert (entry->free == 0);
//     size_t oldsize = entry->size;
//     // if ( (unsigned long)oldsize > (unsigned long)2*size && (oldsize - size)> 1024/*THRESHOLD*/) {
//     //     metadata_entry_t* newentry = entry + size;
//     //     newentry->ptr = newentry + 1;
//     //     newentry->free = 1;
//     //     newentry->size = size - oldsize - sizeof(entry);
//     //     newentry->next = entry->next;
//     //     entry->next = newentry;
//     // }
//     if (oldsize >= size) {
//         return ptr;
//     }
//     void* result = malloc(size);
//     ssize_t minsize = size < oldsize ? (ssize_t)size : oldsize;
//     memcpy(result, ptr, minsize);
//     free(ptr);
//     return result;
// }

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <assert.h>

// typedef struct _metadata_entry_t {
//     void *ptr;
//     size_t size;
//     int free;
//     struct _metadata_entry_t *next;
//     struct _metadata_entry_t *prev;
// } metadata_entry_t;

// static metadata_entry_t *head = NULL;
// static size_t total_memory_requested = 0;
// static size_t total_memory_sbrk = 0;

// // Add a forward declaration for the splitBlock function
// int splitBlock(size_t size, metadata_entry_t *entry);

// void coalescePrev(metadata_entry_t *p);
// void coalesceBlock(metadata_entry_t *p);

// void *calloc(size_t num, size_t size) {
//     // implement calloc!
//     size_t total = num * size;
//     void *result = malloc(total);
//     if (!result) return NULL;
//     memset(result, 0, total);
//     return result;
// }

// int splitBlock(size_t size, metadata_entry_t *entry) {
//     if (entry->size >= 2 * size && (entry->size - size) >= 1024) {
//         metadata_entry_t *new_entry = entry->ptr + size;
//         new_entry->ptr = (new_entry + 1);
//         new_entry->free = 1;
//         new_entry->size = entry->size - size - sizeof(metadata_entry_t);
//         new_entry->next = entry;
//         if (entry->prev) {
//             entry->prev->next = new_entry;
//         } else {
//             head = new_entry;
//         }
//         new_entry->prev = entry->prev;
//         entry->size = size;
//         entry->prev = new_entry;
//         return 1;
//     }
//     return 0;
// }

// void *malloc(size_t size) {
//     // implement malloc!
//     metadata_entry_t *p = head;
//     metadata_entry_t *chosen = NULL;

//     while (p != NULL) {
//         if (p->free && p->size >= size) {
//             chosen = p;
//             if (splitBlock(size, p)) {
//                 total_memory_requested += sizeof(metadata_entry_t);
//             }
//             break;
//         }
//         p = p->next;
//     }
//     if (chosen) {
//         chosen->free = 0;
//         total_memory_requested += chosen->size;
//     } else {
//         if (head && head->free) {
//             if (sbrk(size - head->size) == (void *)-1)
//                 return NULL;
//             total_memory_sbrk += size - head->size;
//             head->size = size;
//             head->free = 0;
//             chosen = head;
//             total_memory_requested += head->size;
//         } else {
//             chosen = sbrk(sizeof(metadata_entry_t) + size);
//             if (chosen == (void *)-1)
//                 return NULL;
//             chosen->ptr = chosen + 1;
//             chosen->size = size;
//             chosen->free = 0;
//             chosen->next = head;
//             if (head) {
//                 chosen->prev = head->prev;
//                 head->prev = chosen;
//             } else {
//                 chosen->prev = NULL;
//             }
//             head = chosen;
//             total_memory_sbrk += sizeof(metadata_entry_t) + size;
//             total_memory_requested += sizeof(metadata_entry_t) + size;
//         }
//     }
//     return chosen->ptr;
// }

// void coalescePrev(metadata_entry_t *p) {
//     p->size += p->prev->size + sizeof(metadata_entry_t);
//     p->prev = p->prev->prev;
//     if (p->prev) {
//         p->prev->next = p;
//     } else {
//         head = p;
//     }
// }

// void coalesceBlock(metadata_entry_t *p) {
//     if (p->prev && p->prev->free == 1) {
//         coalescePrev(p);
//         total_memory_requested -= sizeof(metadata_entry_t);
//     }
//     if (p->next && p->next->free == 1) {
//         p->next->size += p->size + sizeof(metadata_entry_t);
//         p->next->prev = p->prev;
//         if (p->prev) {
//             p->prev->next = p->next;
//         } else {
//             head = p->next;
//         }
//         total_memory_requested -= sizeof(metadata_entry_t);
//     }
// }

// void free(void *ptr) {
//     if (!ptr) return;
//     metadata_entry_t *p = head;
//     while (p) {
//         if (p->ptr == ptr) {
//             p->free = 1;
//             total_memory_requested -= p->size;
//             coalesceBlock(p);
//             return;
//         }
//         p = p->next;
//     }
// }

// void *realloc(void *ptr, size_t size) {
//     // implement realloc!
//     if (!ptr) return malloc(size);
//     metadata_entry_t *entry = ((metadata_entry_t *)ptr) - 1;
//     assert(entry->ptr == ptr);
//     assert(entry->free == 0);
//     if (!size) {
//         free(ptr);
//         return NULL;
//     }
//     if (splitBlock(size, entry)) {
//         total_memory_requested -= entry->prev->size;
//     }
//     if (entry->size >= size) {
//         return ptr;
//     } else if (entry->prev && entry->prev->free && entry->size + entry->prev->size + sizeof(metadata_entry_t) >= size) {
//         total_memory_requested += entry->prev->size;
//         coalescePrev(entry);
//         return entry->ptr;
//     }
//     void *new_ptr = malloc(size);
//     memcpy(new_ptr, ptr, entry->size);
//     free(ptr);
//     return new_ptr;
// }

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

typedef struct _metadata_entry_t {
    void *ptr;
    size_t size;
    int free;
    struct _metadata_entry_t *next;
    struct _metadata_entry_t *prev;
} metadata_entry_t;

static metadata_entry_t *head = NULL;
static size_t total_memory_requested = 0;
static size_t total_memory_sbrk = 0;

void *calloc(size_t num, size_t size) {
    // implement calloc!
    size_t total = num * size;
    void *result = malloc(total);
    if (!result) return NULL;
    memset(result, 0, total);
    return result;
}

void *malloc(size_t size) {
    // implement malloc!
    metadata_entry_t *p = head;
    metadata_entry_t *chosen = NULL;

    while (p != NULL) {
        if (p->free && p->size >= size) {
            chosen = p;
            if (chosen->size >= 2 * size && (chosen->size - size) >= 1024) {
                metadata_entry_t *new_entry = chosen->ptr + size;
                new_entry->ptr = (new_entry + 1);
                new_entry->free = 1;
                new_entry->size = chosen->size - size - sizeof(metadata_entry_t);
                new_entry->next = chosen;
                if (chosen->prev) {
                    chosen->prev->next = new_entry;
                } else {
                    head = new_entry;
                }
                new_entry->prev = chosen->prev;
                chosen->size = size;
                chosen->prev = new_entry;
                total_memory_requested += sizeof(metadata_entry_t);
            }
            break;
        }
        p = p->next;
    }
    if (chosen) {
        chosen->free = 0;
        total_memory_requested += chosen->size;
    } else {
        if (head && head->free) {
            if (sbrk(size - head->size) == (void *)-1)
                return NULL;
            total_memory_sbrk += size - head->size;
            head->size = size;
            head->free = 0;
            chosen = head;
            total_memory_requested += head->size;
        } else {
            chosen = sbrk(sizeof(metadata_entry_t) + size);
            if (chosen == (void *)-1)
                return NULL;
            chosen->ptr = chosen + 1;
            chosen->size = size;
            chosen->free = 0;
            chosen->next = head;
            if (head) {
                chosen->prev = head->prev;
                head->prev = chosen;
            } else {
                chosen->prev = NULL;
            }
            head = chosen;
            total_memory_sbrk += sizeof(metadata_entry_t) + size;
            total_memory_requested += sizeof(metadata_entry_t) + size;
        }
    }
    return chosen->ptr;
}

void free(void *ptr) {
    if (!ptr) return;
    metadata_entry_t *p = head;
    while (p) {
        if (p->ptr == ptr) {
            p->free = 1;
            total_memory_requested -= p->size;

            // Coalesce previous and next blocks if they are free
            if (p->prev && p->prev->free == 1) {
                p->size += p->prev->size + sizeof(metadata_entry_t);
                p->prev = p->prev->prev;
                if (p->prev) {
                    p->prev->next = p;
                } else {
                    head = p;
                }
                total_memory_requested -= sizeof(metadata_entry_t);
            }
            if (p->next && p->next->free == 1) {
                p->next->size += p->size + sizeof(metadata_entry_t);
                p->next->prev = p->prev;
                if (p->prev) {
                    p->prev->next = p->next;
                } else {
                    head = p->next;
                }
                total_memory_requested -= sizeof(metadata_entry_t);
            }
            return;
        }
        p = p->next;
    }
}

void *realloc(void *ptr, size_t size) {
    // implement realloc!
    if (!ptr) {
        return malloc(size);
    }
    metadata_entry_t *entry = ((metadata_entry_t *)ptr) - 1;
    assert(entry->ptr == ptr);
    assert(entry->free == 0);
    if (!size) {
        free(ptr);
        return NULL;
    }
    if (entry->size >= size) {
        return ptr;
    } else if (entry->prev && entry->prev->free && entry->size + entry->prev->size + sizeof(metadata_entry_t) >= size) {
        total_memory_requested += entry->prev->size;
        entry->size += entry->prev->size + sizeof(metadata_entry_t);
        entry->prev = entry->prev->prev;
        if (entry->prev) {
            entry->prev->next = entry;
        } else {
            head = entry;
        }
        return entry->ptr;
    }
    void *new_ptr = malloc(size);
    if (!new_ptr) return NULL;
    memcpy(new_ptr, ptr, entry->size);
    free(ptr);
    return new_ptr;
}
