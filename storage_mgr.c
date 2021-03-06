#include "storage_mgr.h"
#include<stdio.h>
#include<stdlib.h>
#include "dberror.h"
FILE *file;
int pgread;

//initate Storage Manager
void initStorageManager()
{
    printf("Sucessfully Initate Storage Manager\n");
    printf("We will create Page File now\n");
}
// create new file of one page

RC createPageFile (char *filename)
{
    printf("Creating Page File");
    int i;
    char c;
    file=fopen(filename,"r");
    if(file != NULL) //check file content
    {
        printf("File exist.\n");
        printf("Want to replace the content?(y/n)"); //ask to overwrite if file not empty
        scanf("%c",&c);
        fclose(file);
    }
    if (file == NULL) // create file with new content
    {
        file=fopen(filename,"w");
        char *pointer=(char *)calloc(PAGE_SIZE,sizeof(char));
        fwrite(pointer,PAGE_SIZE,sizeof(char),file);
        fseek(file,0,SEEK_END);
        free(pointer);
        fclose(file);
        RC_message="File Created";
        return RC_OK;
    }
      else if(c=='y') //overwrite file
    {
            file=fopen(filename,"w");
            char *pointer=(char *)calloc(PAGE_SIZE,sizeof(char));
            fwrite(pointer,PAGE_SIZE,sizeof(char),file);
            fseek(file,0,SEEK_END);
            free(pointer);
            fclose(file);
            RC_message="Content replaced sucessfully";
            return RC_OK;
     }
      else if(c=='Y')
	{
            file=fopen(filename,"w");
            char *pointer=(char *)calloc(PAGE_SIZE,sizeof(char));
            fwrite(pointer,PAGE_SIZE,sizeof(char),file);
            fseek(file,0,SEEK_END);
            free(pointer);
            fclose(file);
            RC_message="Content replaced sucessfully";
            return RC_OK;
	}	

    else //if user choose not to overwrite.
    {

        RC_message="File already exist. You chose not to overwrite the file.";
        return RC_FILE_EXISTS;
    }
   
}
RC openPageFile(char *fname,SM_FileHandle *fileHandle)
{
    printf("Will open a page file now");
    fileHandle->mgmtInfo=fopen(fname,"r");
    int filepgcnt,filesize;
	printf("after first handle\n");
    if(fileHandle->mgmtInfo==NULL)
    {
        RC_message ="File does not exist";
        return RC_FILE_NOT_FOUND;
    }

    else
    {

      	fileHandle->mgmtInfo=fopen(fname,"r");
       	fileHandle->fileName = fname ;
        fileHandle->curPagePos=0; //update current Page pointer to beginning of file
        fseek(fileHandle->mgmtInfo,0,SEEK_END);//end of the file
        filesize=ftell(fileHandle->mgmtInfo);//file size
        filepgcnt=filesize/PAGE_SIZE; // total number of pages in the file
        fileHandle->totalNumPages=filepgcnt;
        fclose(fileHandle->mgmtInfo);
        RC_message="File Opened Successfully";
        return RC_OK;
    }


}
RC closePageFile (SM_FileHandle *filehandle)
{
	filehandle->mgmtInfo = fopen(filehandle->fileName,"r");	
	printf("Closing File");
	if(fclose(filehandle->mgmtInfo)==0)
	{
		printf("Please wait. While file is closing....\n");
		RC_message="File closed successfully";
		return RC_OK;
	}
	else
		return RC_FILE_NOT_FOUND;
}
RC destroyPageFile (char *filename)
{
		fopen(filename,"r");

		if (remove(filename)==0)//if file is there, remove the file
		{
		printf("File deleted....");
		RC_message="File removed successfully.";
		return RC_OK;
		}
		else
			return RC_FILE_NOT_FOUND;
}

RC readBlock (int pageNum, SM_FileHandle *filehandle, SM_PageHandle memPage)
{
	printf("Reading FIle:%d",pageNum);
	filehandle->mgmtInfo = fopen(filehandle->fileName,"r");
	if(filehandle ==NULL)
	{
		RC_message="Requested File is not initalized";
		return RC_FILE_HANDLE_NOT_INIT;
	}
	else 
	{
		if (filehandle->mgmtInfo ==NULL)
		{
			RC_message="File Not Found";
			return RC_FILE_NOT_FOUND;
		}
		else 
		{
			if((pageNum+1) > filehandle->totalNumPages || pageNum < 0)
			{
				printf("pageNUm:%d",pageNum);
				RC_message="Requested page to be read doesn't exist";
				return RC_READ_NON_EXISTING_PAGE;
			}
			else
			{
				if(memPage !=NULL)
				{
				  fseek(filehandle->mgmtInfo,(((pageNum)*PAGE_SIZE)),SEEK_SET);//seeking to requested page number.
				  fread(memPage,sizeof(char),PAGE_SIZE,filehandle->mgmtInfo);//reading to requested page.
				  filehandle->curPagePos=pageNum;//updating current page position
				  RC_message="File Read Successfull.";
				  fclose(filehandle->mgmtInfo);
				  return RC_OK;
				}		
			}
		}
	}

}
int getBlockPos (SM_FileHandle *filehandle)
{
	if(filehandle ==NULL)
	{	
		RC_message="desired file related data is not initialized";
		return RC_FILE_HANDLE_NOT_INIT;
	}
	else
	{
		if((fopen(filehandle->fileName,"r")) == NULL)
		{
			RC_message="File Not Found"; 
			return RC_FILE_NOT_FOUND;
		}
		else
		{
			printf("%d ",filehandle->curPagePos);
			return filehandle->curPagePos;
		}
	}	
}
RC readFirstBlock (SM_FileHandle *filehandle, SM_PageHandle memPage)
{
	pgread=0;
	readBlock(pgread,filehandle, memPage);
}
RC readPreviousBlock (SM_FileHandle *filehandle, SM_PageHandle memPage)
{
	pgread=getBlockPos(filehandle)-1;
	readBlock (pgread,filehandle, memPage);
}
RC readCurrentBlock (SM_FileHandle *filehandle, SM_PageHandle memPage)
{
	pgread=getBlockPos(filehandle);
	readBlock (pgread,filehandle, memPage);
	
}
RC readNextBlock (SM_FileHandle *filehandle, SM_PageHandle memPage)
{
	pgread=getBlockPos(filehandle)+1;
	readBlock (pgread,filehandle, memPage);
}
RC readLastBlock (SM_FileHandle *filehandle, SM_PageHandle memPage)
{
	pgread=(filehandle->totalNumPages)-1;
	readBlock (pgread,filehandle, memPage);
}
RC writeBlock (int pageNum, SM_FileHandle *filehandle, SM_PageHandle memPage)
{
 	if(filehandle ==NULL)
	{
		RC_message="Requested File data not found;";
		return RC_FILE_HANDLE_NOT_INIT;
	}
	else
	{
		filehandle->mgmtInfo=fopen(filehandle->fileName,"r+");
		if((filehandle->mgmtInfo) == NULL)
		{
			RC_message="File not found";
			return RC_FILE_NOT_FOUND;
		}
		else
		{
			if(pageNum > (filehandle->totalNumPages))
			{
				RC_message="Requested page doesnt exists.";
				return 800;
			}
			else
			{
				if(memPage != NULL)
				{
					//file = fopen(filehandle->fileName,"r+");
                            		fseek(filehandle->mgmtInfo, pageNum*PAGE_SIZE , SEEK_SET);
					if(fwrite(memPage,PAGE_SIZE,1,filehandle->mgmtInfo) == 1)
					{
						//file = fopen(filehandle->fileName,"r");
    						filehandle->curPagePos = (ftell(filehandle->mgmtInfo)/PAGE_SIZE);
   						 fseek(filehandle->mgmtInfo, (filehandle->curPagePos)*PAGE_SIZE , SEEK_SET);
   						 //filehandle->mgmtInfo = file;
						 fclose(filehandle->mgmtInfo);
  						 RC_message="Data write successful.";
						 return RC_OK;
					}
					else
					{
						RC_message="Write Request Failed";
						return RC_WRITE_FAILED;
					}
				}
			}
		}
	}

}

RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
 return writeBlock(fHandle->curPagePos,fHandle,memPage);
}

RC appendEmptyBlock (SM_FileHandle *fHandle)
{
	fHandle->mgmtInfo=fopen(fHandle->fileName,"a");
	if(fHandle == NULL) 
		return RC_FILE_NOT_FOUND;
	else if( fHandle->mgmtInfo == NULL)
		return RC_FILE_HANDLE_NOT_INIT;
	else
	{
		fHandle->totalNumPages+=1;
		fseek(fHandle->mgmtInfo,(((fHandle->totalNumPages)*PAGE_SIZE)),SEEK_END);
		char *len = (char*)calloc(PAGE_SIZE,sizeof(char));
		fwrite(len,PAGE_SIZE,sizeof(char),fHandle->mgmtInfo);
		free(len);
		fclose(fHandle->mgmtInfo);
		return RC_OK;
	
	}
    }
RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle)
{
fHandle->mgmtInfo = fopen(fHandle->fileName,"r");
if(fHandle ==NULL)
	return RC_FILE_NOT_FOUND;
else if (fHandle->mgmtInfo ==NULL)
	return RC_FILE_HANDLE_NOT_INIT;
else if(fHandle->totalNumPages < numberOfPages)
{
	for(;fHandle->totalNumPages < numberOfPages;)
	{
		fclose(fHandle->mgmtInfo);
		appendEmptyBlock(fHandle);
	}
}
else
	return RC_NUMBER_OF_PAGES_EXISTS;
}
 


