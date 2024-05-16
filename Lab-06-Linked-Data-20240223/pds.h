#ifndef PDS_H
#define PDS_H

// Error codes
#define PDS_SUCCESS 0
#define PDS_FILE_ERROR 1
#define PDS_ADD_FAILED 2
#define PDS_REC_NOT_FOUND 3
#define PDS_LOAD_NDX_FAILED 4
#define PDS_REPO_ALREADY_OPEN 12
#define PDS_NDX_SAVE_FAILED 13
#define PDS_REPO_NOT_OPEN 14
#define PDS_DELETE_FAILED 15
#define PDS_LINK_FAILED 16

#include<stdio.h>
#include "bst.h"

// Repository status values
#define PDS_REPO_OPEN 10
#define PDS_REPO_CLOSED 11

struct PDS_NdxInfo{ // NO CHANGE
	int key;
	int offset;
	int is_deleted; // 0 - not deleted, 1 - deleted
};

struct PDS_RepoInfo{
	char pds_name[30];
	FILE *pds_data_fp;
	FILE *pds_linked_data_fp;  // NEW
	FILE *pds_link_fp; // NEW
	FILE *pds_ndx_fp;
	int repo_status; 
	int rec_size; // For fixed length records for main repo
	int linked_rec_size; // NEW
	struct BST_Node *pds_bst;
	int rec_count;
};

// NEW 
// Structure to link parent and child keys
struct PDS_link_info{
	int parent_key;
	int child_key;
};

extern struct PDS_RepoInfo repo_handle;

// pds_create - CHANGED
// Create empty repo data file (.dat) and index file (.ndx) 
// Create empty linked repo data file (.dat) only. No need for .ndx file
// Create empty link data file (.dat) by concatenating the two repo names
// Example: if repo_name is "student" and linked_repo_name is "course", then link data file should be named student_course.dat
int pds_create(char *repo_name, char *linked_repo_name);

// pds_open - CHANGED
// Open the main data file and index file in rb+ mode
// If linked_repo_name is NOT NULL
//     Open the linked data file in rb+ mode (there is no index file for linked data)
//     Open the link file in rb+ mode
// end if
// Update the fields of PDS_RepoInfo appropriately
// Build BST and store in pds_bst by reading index entries from the index file
// Close only the index file
int pds_open( char *repo_name, char *linked_repo_name, int rec_size, int linked_rec_size );

// pds_load_ndx - NO CHANGE
// Internal function used by pds_open to read index entries into BST
int pds_load_ndx();

// put_rec_by_key - NO CHANGE
// Seek to the end of the data file
// Create an index entry with the current data file location using ftell
// Add index entry to BST using offset returned by ftell
// Write the key at the current data file location
// Write the record after writing the key
int put_rec_by_key( int key, void *rec );

// put_linked_rec_by_key - NEW
// Seek to the end of the linked data file
// No need to create index entry
// Write the key at the current data file location
// Write the record after writing the key
int put_linked_rec_by_key( int key, void *rec );


// get_rec_by_key - NO CHANGE
// Search for index entry in BST
// Seek to the file location based on offset in index entry
// Read the key at the current file location 
// Read the record after reading the key
int get_rec_by_ndx_key( int key, void *rec );

// search by non_ndx key - NO CHANGE
int get_rec_by_non_ndx_key(void *key, void *rec, int (*matcher)(void *rec, void *key), int *io_count);

// get_linked_rec_by_key - NEW
// Do a linear search of the given key in the linked data file
int get_linked_rec_by_key( int key, void *rec );
/*
This function returns the corresponding ndx key for the given record at a particular io_count.
Let us say we know that we find a particular record at io_count = k.
Then, in the .dat file the location of this record will be = k * repo_handle.rec_size.
Thus, using fseek() we can set the repo_handle.pds_data_fp at this point.
Now we will read only the key/id (ndx key) of the record and return it.
If fseek() does not return 0 then we return PDS_FILE_ERROR.
*/

// delete by ndx_key - NO CHANGE
// NOTE: When we delete a key, we need to actually delete the linked records too.
// But we will ignore that part for now. 
// This function will now delete only from the main data
int delete_rec_by_ndx_key( int key );
/*
NOTE - We will not decrement the repo_handle.rec_count by 1. Let us understand the reason behind by this through an example.
Assume the key-offset-is_deleted in the .ndx file looks like this:
10000 10 0
10001 20 0
10002 30 0
10003 40 0
10004 50 0
Currently, repo_handle.rec_count is 5. Now, let us say when we want to delete the record with key 10001. Now, if after toggling the is_deleted attribute the record with key 10001 to 1, we decrement the repo_handle.rec_count by 1. Let us say in the next query, we want to read the record with key 10004. Now our repo_handle.rec_count is 4. Now, we would never be able to reach the end of the file because our loop will run 4 times to search but our required record can only be accessed in the 5th iteration. Thus, we should not decrement the repo_handle.rec_count.
*/

// pds_link_rec - NEW
// Create PDS_link_info instance based on key1 and key2
// Go to the end of the link file
// Store the PDS_link_info record
int pds_link_rec (int key1, int key2);

// pds_get_linked_rec
// Go to the beginning of the link file
// Reset result_set_size to 0
// Do a linear search of all link_info records for matching the given parent_key
// Store the matching linked key in linked_keys_result array
// Keep updating the result_set_size
int pds_get_linked_rec(int parent_key, int linked_keys_result[], int *result_set_size);

// pds_close - CHANGED
// Open the index file in wb mode (write mode, not append mode)
// Unload the BST into the index file by traversing it in PRE-ORDER (overwrite the entire index file)
// Free the BST by call bst_destroy()
// Close the index file, data file and linked data file
int pds_close();


#endif
