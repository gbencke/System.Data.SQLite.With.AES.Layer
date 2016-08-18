#define EMPTY_CHAR_FILE 0
#define ENCRYPTION_PAGE_SIZE 4096


unsigned char iv[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
int SizeOfDBFileInPages = 0;
HANDLE hMutex = NULL;

void EncryptPage(char *output, char *input, int length){
	switch(CurrentEncryptionMethod){
		case ENCRYPTION_METHOD_AES:
			AES128_CBC_encrypt_buffer(output,input, length, MVSEncryptionKey, iv);
			break;
		default:
			memcpy(output,input,length);
			break;
	}
}

void DecryptPage(char *output, char *input, int length){
	switch(CurrentEncryptionMethod){
		case ENCRYPTION_METHOD_AES:
			AES128_CBC_decrypt_buffer(output,input, length, MVSEncryptionKey, iv);
			break;
		default:
			memcpy(output,input,length);
			break;
	}
}


void AddPagesToFile(winFile *file , int Pages){
	unsigned char *PagesToAdd;
	unsigned char *PagesToAddEncrypted;
	int PageToAddSizeInBytes = ENCRYPTION_PAGE_SIZE * Pages;
	int ret;

	PagesToAdd = malloc(PageToAddSizeInBytes * 2);
	PagesToAddEncrypted = malloc(PageToAddSizeInBytes * 2);
	memset (PagesToAdd,EMPTY_CHAR_FILE,PageToAddSizeInBytes);

	EncryptPage(PagesToAddEncrypted,PagesToAdd, PageToAddSizeInBytes);
	//AES128_CBC_encrypt_buffer(PagesToAddEncrypted,PagesToAdd, PageToAddSizeInBytes, key, iv);

	ret = winWrite(file,PagesToAddEncrypted,PageToAddSizeInBytes,SizeOfDBFileInPages * ENCRYPTION_PAGE_SIZE);
	free(PagesToAdd);
	free(PagesToAddEncrypted);
	
	return ret;
}

int winWriteEncrypted(
  void *id,               /* File to write into */
  const void *pBuf,               /* The bytes to be written */
  int amt,                        /* Number of bytes to write */
  __int64 offset            /* Offset into the file to begin writing at */
){
	winFile *file = (winFile *)id;
	
	MVS_logToTextFile(file->zPath);

	if(hMutex==NULL){
		hMutex = osCreateMutexW(NULL,FALSE,"E");
	}

	WaitForSingleObject(hMutex,INFINITE);

	if(!strstr(file->zPath,"-journal")){
		int ret;
		char tmpStr[1000];
		int StartPage = offset / ENCRYPTION_PAGE_SIZE;
		int EndPage = StartPage + (amt / ENCRYPTION_PAGE_SIZE) + (amt % ENCRYPTION_PAGE_SIZE > 0 ? 1 : 0);
		__int64 FinalPosInFile = offset + amt;
		int WritePages = (EndPage-StartPage);
		int WriteBufferSize = WritePages * ENCRYPTION_PAGE_SIZE;
		char *BufferToWrite;
		char *bufferEncrypted;
		char *bufferDecrypted;

		MVS_logToTextFile("This is not a journal file write");

		sprintf(tmpStr,"Offset: %ld", offset);
		MVS_logToTextFile(tmpStr);
		sprintf(tmpStr,"amt: %ld", amt);
		MVS_logToTextFile(tmpStr);
		sprintf(tmpStr,"Start Page:%d ", StartPage );
		MVS_logToTextFile(tmpStr);
		sprintf(tmpStr,"End Page:%d", EndPage );
		MVS_logToTextFile(tmpStr);

		if((FinalPosInFile / ENCRYPTION_PAGE_SIZE) > SizeOfDBFileInPages){
			char NewFileSize[1000];
			int NewSizeOfDBFileInPages =  FinalPosInFile / ENCRYPTION_PAGE_SIZE;
			int NumberOfPagesToAdd = NewSizeOfDBFileInPages - SizeOfDBFileInPages;

			if(FinalPosInFile % ENCRYPTION_PAGE_SIZE) // Account for extra bytes after page...
				NewSizeOfDBFileInPages++;
			
			AddPagesToFile(file, NumberOfPagesToAdd);
			
			SizeOfDBFileInPages = NewSizeOfDBFileInPages;

			sprintf(NewFileSize,"New File Size:%d pages",SizeOfDBFileInPages);
			MVS_logToTextFile(NewFileSize);
		}

		BufferToWrite = malloc(WriteBufferSize * 2);
		bufferEncrypted = malloc(WriteBufferSize * 2);
		bufferDecrypted = malloc(WriteBufferSize * 2);

		ret = winRead(id,bufferEncrypted,WriteBufferSize,StartPage * ENCRYPTION_PAGE_SIZE);

		DecryptPage(bufferDecrypted,bufferEncrypted, WriteBufferSize);
		//AES128_CBC_decrypt_buffer(bufferDecrypted,bufferEncrypted, WriteBufferSize, key, iv);

		memcpy(&bufferDecrypted[offset %  ENCRYPTION_PAGE_SIZE],pBuf,amt);

		EncryptPage(BufferToWrite,bufferDecrypted, WriteBufferSize);
		//AES128_CBC_encrypt_buffer(BufferToWrite,bufferDecrypted, WriteBufferSize, key, iv);

		ret = winWrite(id,BufferToWrite,WriteBufferSize,StartPage * ENCRYPTION_PAGE_SIZE);
		free(BufferToWrite);
		free(bufferEncrypted);
		free(bufferDecrypted);
		ReleaseMutex(hMutex);
		return ret;
	}
	ReleaseMutex(hMutex);
	return winWrite(id,pBuf,amt,offset);
	
}



int winReadEncrypted(
  void *id,               /* File to write into */
  const void *pBuf,               /* The bytes to be written */
  int amt,                        /* Number of bytes to write */
  __int64 offset            /* Offset into the file to begin writing at */
){
	winFile *file = (winFile *)id;
	int StartPage = offset / ENCRYPTION_PAGE_SIZE;
	int EndPage = StartPage + (amt / ENCRYPTION_PAGE_SIZE) + (amt % ENCRYPTION_PAGE_SIZE > 0 ? 1 : 0);
	int NumberPagesToRead = (EndPage-StartPage);
	int ReadBufferSize = NumberPagesToRead * ENCRYPTION_PAGE_SIZE;
	char *BufferToRead;
	char *bufferDecrypted;
	char *bufferEncrypted;
	int ret;

	if(hMutex==NULL){
		hMutex = osCreateMutexW(NULL,FALSE,"E");
	}

	WaitForSingleObject(hMutex,INFINITE);

	if(!strstr(file->zPath,"-journal")){
		BufferToRead = malloc(ReadBufferSize);
		ret = winRead(id,(const void *)BufferToRead,ReadBufferSize,StartPage * ENCRYPTION_PAGE_SIZE);
		
		bufferEncrypted = malloc(ReadBufferSize* 2);
		bufferDecrypted = malloc(ReadBufferSize* 2);
		memcpy(bufferEncrypted,BufferToRead,ReadBufferSize);

		DecryptPage(bufferDecrypted,bufferEncrypted, ReadBufferSize);
		//AES128_CBC_decrypt_buffer(bufferDecrypted,bufferEncrypted, ReadBufferSize, key, iv);
		memcpy(pBuf,&bufferDecrypted[offset % ENCRYPTION_PAGE_SIZE],amt);
		
		free(BufferToRead);
		free(bufferEncrypted);
		free(bufferDecrypted);
		ReleaseMutex(hMutex);
		return ret;
	}
	ReleaseMutex(hMutex);
	return winRead(id,pBuf,amt,offset);
}

#define LOGTEXTFILE_ENABLED

void MVS_logToTextFile(const char *strToLog){
#ifdef LOGTEXTFILE_ENABLED
	FILE *test = fopen("log.txt","a");
	fprintf(test,"%s\r\n",strToLog);
	fclose(test);
#endif 
}

