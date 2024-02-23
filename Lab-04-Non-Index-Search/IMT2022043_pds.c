#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pds.h"

void bst_print_custom(struct BST_Node *root)
{
    if (root == NULL)
        return;
    else
    {
        fwrite(root->data, sizeof(root->data), 1, repo_handle.pds_ndx_fp);
        bst_print(root->left_child);
        bst_print(root->right_child);
    }
}

struct PDS_RepoInfo repo_handle;

// pds_create
// Open the data file and index file in "wb" mode
// Initialize index file by storing "0" to indicate there are zero entries in index file
// close the files
int pds_create(char *repo_name)
{
    char *filename_dat = (char *)malloc(sizeof(repo_name));
    char *filename_ndx = (char *)malloc(sizeof(repo_name));
    strcpy(filename_dat, repo_name);
    strcpy(filename_ndx, repo_name);
    strcat(filename_dat, ".dat");
    strcat(filename_ndx, ".ndx");
    repo_handle.pds_data_fp = fopen(filename_dat, "wb");
    repo_handle.pds_ndx_fp = fopen(filename_ndx, "wb");
    if (repo_handle.pds_data_fp && repo_handle.pds_ndx_fp)
    {
        int zero = 0;
        fwrite(&zero, sizeof(int), 1, repo_handle.pds_ndx_fp);
        fclose(repo_handle.pds_data_fp);
        fclose(repo_handle.pds_ndx_fp);
        repo_handle.repo_status = PDS_REPO_CLOSED;
        free(filename_dat);
        free(filename_ndx);
        return PDS_SUCCESS;
    }
    free(filename_dat);
    free(filename_ndx);
    return PDS_FILE_ERROR;
}

// pds_open
// Open the data file and index file in rb+ mode
// Update the fields of PDS_RepoInfo appropriately
// Read the number of records form the index file
// Load the index into the array and store in ndx_array by reading index entries from the index file
// Close only the index file
int pds_open(char *repo_name, int rec_size)
{
    strcpy(repo_handle.pds_name, repo_name);
    char *filename_dat = (char *)malloc(sizeof(repo_name));
    char *filename_ndx = (char *)malloc(sizeof(repo_name));
    strcpy(filename_dat, repo_name);
    strcpy(filename_ndx, repo_name);
    strcat(filename_dat, ".dat");
    strcat(filename_ndx, ".ndx");
    if (repo_handle.repo_status == PDS_REPO_CLOSED)
    {
        repo_handle.pds_data_fp = fopen(filename_dat, "rb+");
        repo_handle.pds_ndx_fp = fopen(filename_ndx, "rb+");
        repo_handle.rec_size = rec_size;
        if (pds_load_ndx() == PDS_SUCCESS)
        {
            fclose(repo_handle.pds_ndx_fp);
            repo_handle.repo_status = PDS_REPO_OPEN;
            free(filename_dat);
            free(filename_ndx);
            return PDS_SUCCESS;
        }
        else
        {
            repo_handle.repo_status = PDS_REPO_OPEN;
            free(filename_dat);
            free(filename_ndx);
            return PDS_NDX_SAVE_FAILED;
        }
    }
    if (repo_handle.repo_status == PDS_REPO_OPEN)
    {
        free(filename_dat);
        free(filename_ndx);
        return PDS_REPO_ALREADY_OPEN;
    }
    free(filename_dat);
    free(filename_ndx);
    return PDS_FILE_ERROR;
}

// pds_load_ndx - Internal function
// Load the index entries into the BST ndx_root by calling bst_add_node repeatedly for each
// index entry. Unlike array, for BST, you need read the index file one by one in a loop
int pds_load_ndx()
{
    int read_NDX = fread(&repo_handle.rec_count, sizeof(int), 1, repo_handle.pds_ndx_fp);
    struct PDS_NdxInfo *Ndx_Array = (struct PDS_NdxInfo *)malloc(read_NDX * sizeof(struct PDS_NdxInfo));
    fread(Ndx_Array, sizeof(struct PDS_NdxInfo), read_NDX, repo_handle.pds_ndx_fp);
    if (read_NDX == 0)
    {
        free(Ndx_Array);
        return PDS_NDX_SAVE_FAILED;
    }
    for (int i = 0; i < read_NDX; i++)
    {
        bst_add_node(&repo_handle.ndx_root, Ndx_Array[i].key, &(Ndx_Array[i]));
    }
    free(Ndx_Array);
    return PDS_SUCCESS;
}

// put_rec_by_key
// Seek to the end of the data file
// Create an index entry with the current data file location using ftell
// Add index entry to BST by calling bst_add_node
// Increment record count
// Write the record at the end of the file
// Return failure in case of duplicate key
int put_rec_by_key(int key, void *rec)
{
    if (repo_handle.repo_status == PDS_REPO_OPEN)
    {
        if (fseek(repo_handle.pds_data_fp, 0, SEEK_END) == 0)
        {
            struct BST_Node *temp_BST;
            if ((temp_BST = bst_search(repo_handle.ndx_root, key)) == NULL)
            {
                int Offset = ftell(repo_handle.pds_data_fp);
                struct PDS_NdxInfo *temp_Struct = (struct PDS_NdxInfo *)malloc(sizeof(struct PDS_NdxInfo));
                temp_Struct->key = key;
                temp_Struct->offset = Offset;
                repo_handle.rec_count++;
                fwrite(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp);
                int status = bst_add_node(&repo_handle.ndx_root, key, temp_Struct);
                if (status == BST_SUCCESS)
                {
                    return PDS_SUCCESS;
                }
                else
                {
                    return PDS_ADD_FAILED;
                }
            }
            return PDS_ADD_FAILED;
        }
        return PDS_FILE_ERROR;
    }
    else
    {
        return PDS_REPO_NOT_OPEN;
    }
    return PDS_ADD_FAILED;
}

// get_rec_by_ndx_key
// Search for index entry in BST by calling bst_search
// Seek to the file location based on offset in index entry
// Read the record from the current location
int get_rec_by_ndx_key(int key, void *rec)
{
    if (repo_handle.repo_status == PDS_REPO_OPEN)
    {
        struct BST_Node *temp_BST;
        if ((temp_BST = bst_search(repo_handle.ndx_root, key)) != NULL)
        {
            struct PDS_NdxInfo *temp_Struct = (struct PDS_NdxInfo *)(temp_BST->data);
            if (fseek(repo_handle.pds_data_fp, temp_Struct->offset, SEEK_SET) == 0)
            {
                fread(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp);
                return PDS_SUCCESS;
            }
            return PDS_FILE_ERROR;
        }
        return PDS_REC_NOT_FOUND;
    }
    return PDS_REPO_NOT_OPEN;
}

int get_rec_by_non_ndx_key(void *non_ndx_key, void *rec, int (*matcher)(void *rec, void *non_ndx_key), int *io_count)
{
    if (repo_handle.repo_status == PDS_REPO_OPEN)
    {
        if (fseek(repo_handle.pds_data_fp, 0, SEEK_SET) == 0)
        {
            void *temp_Ptr = malloc(repo_handle.rec_size);
            for (int i = 0; i < repo_handle.rec_count; i++)
            {
                (*io_count)++;
                fread(temp_Ptr, repo_handle.rec_size, 1, repo_handle.pds_data_fp);
                if (matcher(temp_Ptr, non_ndx_key) == 0)
                {
                    fseek(repo_handle.pds_data_fp, -repo_handle.rec_size, SEEK_CUR);
                    fread(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp);
                    return 1;
                }
            }
            return PDS_REC_NOT_FOUND;
        }
        return PDS_FILE_ERROR;
    }
    return PDS_REPO_NOT_OPEN;
}

// pds_close
// Open the index file in wb mode (write mode, not append mode)
// Store the number of records
// Unload the ndx_array into the index file by traversing the BST in pre-order mode (overwrite the entire index file)
// Think why should it NOT be in-order?
// Close the index file and data file

int pds_close()
{
    char *temp = (char *)malloc(sizeof(repo_handle.pds_name));
    strcpy(temp, repo_handle.pds_name);
    strcat(temp, ".ndx");
    repo_handle.pds_ndx_fp = fopen(temp, "wb");
    if (repo_handle.pds_ndx_fp != NULL)
    {
        fwrite(&repo_handle.rec_count, sizeof(int), 1, repo_handle.pds_ndx_fp);
        bst_print_custom(repo_handle.ndx_root);
        fclose(repo_handle.pds_data_fp);
        fclose(repo_handle.pds_ndx_fp);
        repo_handle.repo_status = PDS_REPO_CLOSED;
        free(temp);
        return PDS_SUCCESS;
    }

    return PDS_FILE_ERROR;
}
