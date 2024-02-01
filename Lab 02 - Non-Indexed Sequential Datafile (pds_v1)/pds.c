#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include "pds.h"

// Define the global variable
struct PDS_RepoInfo repo_handle;

int pds_create( char *repo_name )
{
    char filename[30];
    strcpy(filename, repo_name);
    strcat(filename, ".dat");   
    FILE *fp = fopen(filename, "wb");
    if(fp == NULL)
    {
        return PDS_FILE_ERROR;
    }
    else{
        fclose(fp);
        return PDS_SUCCESS;
    }
}

int pds_open( char *repo_name, int rec_size  )
{
    char filename[30];
    strcpy(filename, repo_name);
    strcat(filename, ".dat");
    repo_handle.pds_data_fp = fopen(filename, "rb+");
    strcpy(repo_handle.pds_name, filename);
    repo_handle.rec_size = rec_size;
    repo_handle.repo_status = PDS_REPO_OPEN;
    return PDS_SUCCESS;
}

int put_rec_by_key( int key, void *rec )
{
    if (repo_handle.repo_status != PDS_REPO_OPEN)
    {
        return PDS_ADD_FAILED;
    }
    fseek(repo_handle.pds_data_fp, 0, SEEK_END);
    fwrite(&key, 1, sizeof(int), repo_handle.pds_data_fp);
    fwrite(rec, 1, repo_handle.rec_size, repo_handle.pds_data_fp);
    return PDS_SUCCESS;
}

int get_rec_by_key( int key, void *rec )
{
    rewind(repo_handle.pds_data_fp);
    int key1;
    while(fread(&key1, 1, sizeof(int), repo_handle.pds_data_fp)) // while loop will run till fread returns 0(EOF).
    {
        if(key1 == key)
        {
            fread(rec, 1, repo_handle.rec_size, repo_handle.pds_data_fp);
            return PDS_SUCCESS;
        }
        fseek(repo_handle.pds_data_fp, repo_handle.rec_size, SEEK_CUR);
    }
    return PDS_REC_NOT_FOUND;
}

int pds_close()
{
    if(repo_handle.repo_status == PDS_REPO_OPEN)
    {
        fclose(repo_handle.pds_data_fp);
        repo_handle.repo_status = PDS_REPO_CLOSED;
        return PDS_SUCCESS;
    }
    else
    {
        return PDS_FILE_ERROR;
    }
}
