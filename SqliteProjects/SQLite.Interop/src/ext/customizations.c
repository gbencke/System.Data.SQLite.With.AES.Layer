/*
SQLite3 Encryption Layer - 2016 - MVS

First of all, let us start with a FAQ:

-------------------------------------------------------------
What is SQLite3?
SQLite3 is a pure C implementation of a Relational Database contained on a single C file: sqlite3.c with over 200k LOC. 
One of the great features of SQLite3 is that it is easily embeddable in several platforms and be compiled by hundreds
of different compilers, from Microcontrollers, Android, MacOSX, Java, Windows, all use SQLite3 and its single C file.
But, in order to attain such versatily it is important to notice that it uses a VERY OLD C standard, so for instance,
you cannot declare variables after you started doing statements for example:

printf("Print Test:");
int f = 5;
printf("%d:",f);

//The code above is invalid and won't compile, you need to do this way:

int f = 5;
printf("Print Test:");
printf("%d:",f);

SQLite3 is NOT object oriented, and is by default NON-Multithreaded, the sqlite3.c file consists mainly of 2 parts:
1 - Generic Logic: Which is available for all platforms and compilers.
2 - Platform Specific Code: Which is specific for a certain platform or compiler, this code is guarded by several
#define preprocessor diretives that guard code specific for a certain platform. On this project, we will work only
on the code that is defined for WIN32.

-------------------------------------------------------------
What is the objective of the project?
Take System.Data.SQLite which is the official implementation of SQLite for .NET platform and develop some way to
make sure that we can keep the database encrypted at all times using a Encryption algorithm that can be selected for
that specific database and configured for that also.

-------------------------------------------------------------
What are the code components on System.Data.SQLite?
There are 4 Dlls that we are going to use on this project:
1-System.Data.SQLite.DLL - Which is both a Native C DLL and a .NET DLL that contains all SQLite3 implementation
2-System.Data.SQLite.EF6.dll - It is System.Data.SQLite.DLL + support for Entity Framework 6.
3-System.Data.SQLite.Linq.dll - It is System.Data.SQLite.DLL + support for LINQ Queries.
4-SQLite.Designer.DLL - Classes to use SQLite3 Designed in Visual Studio.

-------------------------------------------------------------
What is the chosen solution?
The solution is to:
1 - Identify the functions that do the low-level operating system read/write on the database files and then 
replace them with the functions of the same signature, but that also implement encryption.
2 - Configure this Encryption on the Database by adding 2 more Pragmas to System.Data.SQLite .NET Assembly.

-------------------------------------------------------------
Wait! But System.Data.SQLite.DLL is a .NET DLL, but there are no C# Classes? What is the Catch?
If you look at the unit tests, we use .NET Classes all the time, SQLiteConnection, SQLiteFactory and so on...
But they are not on the project? On the project there are only C files!
Yes, the .NET part was left untouched and if you take a look on the SQLite.Interop.2012 project we link:

$(ProjectDir)..\Libs\System.Data.SQLite\System.Data.SQLite.netmodule;

This netmodule contains all the .NET specific code that is then added to the C code. You will notice
that I also use System.Data.SQLite.EF6 and System.Data.SQLite.Linq.dll directly from the Download site of
SQLIte3 (https://www.sqlite.org/download.html) without any modification.

-------------------------------------------------------------
In a nutshell, what was changed:

1) In sqlite3.c, we added the call to our encrypted-enabled version of the WinRead/WinWrite functions in 
   lines 41310 - 41343 of sqlite3.c.
   IT IS IMPORTANT TO NOTICE THAT THESE NEW FUNCTIONS DO THE ENCRYPTION AND THEN CALL THE
   ORIGINAL LOW LEVEL FUNCTIONS FROM SQLITE3.
2) In sqlite3.c, we added 2 pragmas: PragTyp_ENCRYPTION_METHOD and PragTyp_ENCRYPTION_KEYS on lines
   112075 - 112120 of sqlite3.c
3) For our customized functions we added the file: customizations.c (which is entirely new).
4) We added the file aes.c which is a very tiny AES128 implementation in pure C of the AES algorithm
   and that was downloaded from: https://github.com/kokke/tiny-AES128-C

THAT IS ALL. LESS THAN 300 LINES...

I commented the lines on sqlite3.c mentioned above, so, let us check line by line what we are doing...

Ok, let us first start with some definitions:

Is is important to notice that in order to use the encryption library for AES and actually
any other symmetrical encryption, we need to make sure that we have a file in which size is
a multiple of a certain size, or in other terms, it needs to be PAGED, what is paging? It is 
the technique by which instead of writing in a file by sending only the bytes that we need on the
offset that we need, we are always reading or writing at least 1 page of n bytes. So, let us say
that we have a page of 4096 bytes and we need to write the byte 0xFA on offset 5000 of a file, 
we need to read the page that goes from the 4097th byte to the 8096th byte, place those 4096 bytes 
on a byte array (that we call a buffer), and then modify the 904th Byte to the desired value and then 
write the entire 4096 bytes back to the file, all of this math to read the page, see which byte we 
need and then apply the operation, we call "pointer-arithmetics". 
*/

/*
On some situations that we need to write only a few bytes on the page, we need to PAD the buffer with a 
certain value, this value to pad to the page, is the EMPTY_CHAR_FILE, which in our case is set to 0.
*/
#define EMPTY_CHAR_FILE 0
// Below we define our standard page size:
#define ENCRYPTION_PAGE_SIZE 4096


// For AES we need a initialization vector which is defined below:
unsigned char iv[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

/*
It is important to notice that SQLite3 always use only database at every time, so it is only one integer to keep track
of the size of the database file in SQLite3, the variable below controls the size (in pages) of the current
SQlite3 db being used.
*/
int SizeOfDBFileInPages = 0;

/*
As we use paging, and we might have multiple threads, it is always good to use mutexes, let us initialized our mutex to NULL.
*/
HANDLE hMutex = NULL;

/*
It is important to notice that in C there is not the concept of instances and classes, but of buffers, what is a buffer?
Is a region of memory, normally manually allocated (not being tracked by a garbage collector) and starts at a certain 
address and has a certain length.

The function below, takes a buffer pointed to a certain address called input, and another of output, both have a size
defined by the parameter:length. It is important to notice that there is NO RUNTIME CHECK of index bounds and it the 
parameters are valid or not, and also there is not the Concept of Exceptions, so if the 2 buffers are not correctly
allocated prior to the function below being called, there will be a memory corruption that can make the program instable.
*/

void EncryptPage(char *output, char *input, int length){
	
	/* So, first thing we need to know the algortithm to use...
	   in our case now we have only AES... */
	switch(CurrentEncryptionMethod){
		case ENCRYPTION_METHOD_AES:
			// Call the AES128 function from aes.c file which we downloaded from
			// https://github.com/kokke/tiny-AES128-C and passed also the 
			// Encryption key which is defined by the PragTyp_ENCRYPTION_KEYS PRAGMA in
			// slite3.c file lines: 112096 - 112117
			AES128_CBC_encrypt_buffer(output,input, length, MVSEncryptionKey, iv);
			break;
		default:
			// Ok, if we don't need to encrypt just copy the bytes from input to output
			// without any modification.
			memcpy(output,input,length);
			break;
	}
}


// Same commentaries from EncryptPage apply here, but now we decrypt instead of encrypting.
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

/*

Ok, as we are going to use paging, we have a problem, if the file is 0 bytes in size and we need to write on
the 12000th position, we need to add the pages correctly to the file before we can do any write operation.

This function takes as parameter a struct called winFile which has several informations about the File
that we are trying to manipulate (as seen below), and also the number of pages to be added

struct winFile {
  const sqlite3_io_methods *pMethod; // Must be first 
  sqlite3_vfs *pVfs;      // The VFS used to open this file 
  HANDLE h;               // Handle for accessing the file 
  u8 locktype;            // Type of lock currently held on this file 
  short sharedLockByte;   // Randomly chosen byte used as a shared lock 
  u8 ctrlFlags;           // Flags.  See WINFILE_* below 
  DWORD lastErrno;        // The Windows errno from the last I/O error 
#ifndef SQLITE_OMIT_WAL
  winShm *pShm;           // Instance of shared memory on this file 
#endif
  const char *zPath;      // Full pathname of this file 
  int szChunk;            // Chunk size configured by FCNTL_CHUNK_SIZE 
#if SQLITE_OS_WINCE
  LPWSTR zDeleteOnClose;  // Name of file to delete when closing 
  HANDLE hMutex;          // Mutex used to control access to shared lock 
  HANDLE hShared;         // Shared memory segment used for locking 
  winceLock local;        // Locks obtained by this instance of winFile 
  winceLock *shared;      // Global shared lock memory for the file  
#endif
#if SQLITE_MAX_MMAP_SIZE>0
  int nFetchOut;                // Number of outstanding xFetch references 
  HANDLE hMap;                  // Handle for accessing memory mapping 
  void *pMapRegion;             // Area memory mapped 
  sqlite3_int64 mmapSize;       // Usable size of mapped region 
  sqlite3_int64 mmapSizeActual; // Actual size of mapped region 
  sqlite3_int64 mmapSizeMax;    // Configured FCNTL_MMAP_SIZE value 
#endif
};

*/

int AddPagesToFile(winFile *file , int Pages){
	// Our beloved buffers that will be used to hold the plain data and also the encrypted data.
	unsigned char *PagesToAdd;
	unsigned char *PagesToAddEncrypted;
	// We need to know the size in bytes that we are going to add to the file.
	int PageToAddSizeInBytes = ENCRYPTION_PAGE_SIZE * Pages; 
	// Variable that will hold the value to be returned by the function...
	int ret;

	// First, we need to manually allocate 2 buffers of memory (remember there is no garbage collection or
	// any automatic management of memory, reference counting or any other method), just for safety, I will
	// actually allocate twice the size that we need, there will be a buffer with encrypted data, and another
	// with unencrypted data.
	PagesToAdd = malloc(PageToAddSizeInBytes * 2);
	PagesToAddEncrypted = malloc(PageToAddSizeInBytes * 2);

	// Now that we have 2 buffers, we can now set all the bytes of the buffer with the EMPTY_CHAR value
	// So we are going to have a region of memory with the size = PageToAddSizeInBytes, and starting @
	// PagesToAdd address.
	memset (PagesToAdd,EMPTY_CHAR_FILE,PageToAddSizeInBytes);

	// Okay, now that we have a very homogeneous buffer with all bytes filled with the same value, we
	// can encrypt the buffer and store the result on our other allocated buffer: PagesToAddEncrypted
	EncryptPage(PagesToAddEncrypted,PagesToAdd, PageToAddSizeInBytes);

	// So, we have 2 buffers: one encrypted and the other not encrypted, we can then use the original
	// low level io function from SQLite3 to write the pages on the END OF THE FILE, we calculate the 
	// end of the file by multipling the number of pages on the file by page size:
	// SizeOfDBFileInPages * ENCRYPTION_PAGE_SIZE
	ret = winWrite(file,PagesToAddEncrypted,PageToAddSizeInBytes,SizeOfDBFileInPages * ENCRYPTION_PAGE_SIZE);
	// Deallocate the 2 buffers to avoid a memory leak...
	free(PagesToAdd);
	free(PagesToAddEncrypted);
	// return the error code from the actual write operation.
	return ret;
}


/*
   The function winWriteEncrypted is the function that we developed to write to the SQLite3 Database Files
   using encryption instead of plain information. It takes 4 parameters:
   *id = the winFile Struct (described on comments above) that represent the journal file.
   *pBuf = the Buffer to write (a pointer to the start of the memory region containing the bytes)
   amt = the Number of bytes to write.
   offset = the position on the file where those bytes should be written.
*/

int winWriteEncrypted(
  void *id,               /* File to write into */
  const void *pBuf,               /* The bytes to be written */
  int amt,                        /* Number of bytes to write */
  __int64 offset            /* Offset into the file to begin writing at */
){
	// First, let us cast the pointer to its proper type...
	winFile *file = (winFile *)id;
	
	MVS_logToTextFile(file->zPath);

	// If we don't have a mutex let us create a mutex using
	// the proper SQLite3 native function.
	if(hMutex==NULL){
		hMutex = osCreateMutexW(NULL,FALSE,"E");
	}

	// If we have a mutex, let us wait for it in order to obtain its
	// ownership, just to avoid that 2 threads access the code below at the same time....
	WaitForSingleObject(hMutex,INFINITE);

	// We need to check if the name of the file that we are going to write is not a journal, 
	// those journals are rollback segments used for transactions within SQLite3, they exist
	// only for a few milliseconds and then are destroyed, we are only interested on the 
	// real database files.
	if(!strstr(file->zPath,"-journal")){
		int ret;
		char tmpStr[1000];
		// First thing, we need of course to know what is the page that we are going to start writing.
		// So we get the offset and divide it by the size of each page, the int datatype will truncate
		// the remainder on the division, so no problem about that...
		int StartPage = offset / ENCRYPTION_PAGE_SIZE;
		// Later we need to calculate what is the last page that we will access, we get the startpage,
		// then we how many pages we need for the amt of bytes and we add a remainder just in case
		// that we offshoot the final page.
		int EndPage = StartPage + (amt / ENCRYPTION_PAGE_SIZE) + (amt % ENCRYPTION_PAGE_SIZE > 0 ? 1 : 0);
		// We calculate the ABSOLUTE last position on the file.
		__int64 FinalPosInFile = offset + amt;
		// And we calculare the number of pages that we need to write.
		int WritePages = (EndPage-StartPage);
		// And we calculate the size in bytes that we will need for our buffers...
		int WriteBufferSize = WritePages * ENCRYPTION_PAGE_SIZE;

		// Ok, these are our buffers...
		char *BufferToWrite;
		char *bufferEncrypted;
		char *bufferDecrypted;

		// Begin Debug logs... Debug logs... Debug logs... Debug logs... Debug logs... Debug logs...
		MVS_logToTextFile("This is not a journal file write");
		sprintf(tmpStr,"Offset: %ld", offset);
		MVS_logToTextFile(tmpStr);
		sprintf(tmpStr,"amt: %ld", amt);
		MVS_logToTextFile(tmpStr);
		sprintf(tmpStr,"Start Page:%d ", StartPage );
		MVS_logToTextFile(tmpStr);
		sprintf(tmpStr,"End Page:%d", EndPage );
		MVS_logToTextFile(tmpStr);
		// End Debug logs... Debug logs... Debug logs... Debug logs... Debug logs... Debug logs...

		// First decision that we need to do, are we writting in a offset 
		// that is already on the file, are we trying to write to let us say
		// byte 50000 and the file has only 20000 bytes in size?
		if((FinalPosInFile / ENCRYPTION_PAGE_SIZE) > SizeOfDBFileInPages){
			// If so, we need to add pages to that file.
			char NewFileSize[1000];
			// Calculate the new file size in pages.
			int NewSizeOfDBFileInPages =  FinalPosInFile / ENCRYPTION_PAGE_SIZE;
			// We calculate how many we need to add.
			int NumberOfPagesToAdd = NewSizeOfDBFileInPages - SizeOfDBFileInPages;
			// Do we need to account for 1 more page?
			if(FinalPosInFile % ENCRYPTION_PAGE_SIZE) // Account for extra bytes after page...
				NewSizeOfDBFileInPages++;
			//Ok, now we do add the pages to the file...
			AddPagesToFile(file, NumberOfPagesToAdd);
			//And we get the new size of the main database file...
			SizeOfDBFileInPages = NewSizeOfDBFileInPages;
			// Debug logs... Debug logs... Debug logs... Debug logs... Debug logs... Debug logs...
			sprintf(NewFileSize,"New File Size:%d pages",SizeOfDBFileInPages);
			MVS_logToTextFile(NewFileSize);
		}

		// Now that we are sure that we have the file on the correct size, 
		// we are going to allocate the buffers that we need to actually perform the write
		// bufferEncrypted is the buffer that CONTAINS THE ENCRYPTED DATA THAT IS CURRENTLY WRITTEN ON THE DATABASE FILE
		bufferEncrypted = malloc(WriteBufferSize * 2);
		// bufferDecrypted contains the DECRYPTED DATA THAT IS CURRENTLY WRITTEN ON THE DATABASE FILE.
		bufferDecrypted = malloc(WriteBufferSize * 2);
		// BufferToWrite takes bufferDecrypted and writes the data that we need to modify on bufferDecrypted, then
		// we have a buffer with the actual content of the page(s) that we need to write to the disk. On the
		// end we encrypt BufferToWrite and this is the buffer that goes to the database file.
		BufferToWrite = malloc(WriteBufferSize * 2);
		
		
		// First thing, we need to use the ORIGINAL SQLite3 function to read the ACTUAL content of the page
		// that we need to modify, we will read thbe actual write buffer (WriteBufferSize) size, starting at the 
		// necessary start page: StartPage * ENCRYPTION_PAGE_SIZE
		ret = winRead(id,bufferEncrypted,WriteBufferSize,StartPage * ENCRYPTION_PAGE_SIZE);
		// Ok, we have a buffer, read directly from the disk, but it is encrypted, so we need to 
		// decrypt it...
		DecryptPage(bufferDecrypted,bufferEncrypted, WriteBufferSize);
		// Now that we have a decrypted buffer, we need to copy the data that we need to write on the 
		// exact location on the buffer that we just read and decrypted, so we are going to do some
		// pointer arithmetics to get the correct byte to start copying on the buffer and then 
		// cópy "amt" bytes from "pBuf" on bufferDecrypted
		memcpy(&bufferDecrypted[offset %  ENCRYPTION_PAGE_SIZE],pBuf,amt);
		// Ok, now we have the final buffer ready, as we read the data from the disk and then 
		// copied the new data into the correct position with cirurgical precision.
		// We now encrypt the data, and...
		EncryptPage(BufferToWrite,bufferDecrypted, WriteBufferSize);
		// We write the final encrypted modified data back into the disk...
		ret = winWrite(id,BufferToWrite,WriteBufferSize,StartPage * ENCRYPTION_PAGE_SIZE);
		// Dellocate all buffers.
		free(BufferToWrite);
		free(bufferEncrypted);
		free(bufferDecrypted);
		// Release the mutex, so now it is same for the waiting thread(s) to
		// execute this operation.
		ReleaseMutex(hMutex);
		// And return the sucess code from our write operation.
		return ret;
	}
	// In the case that we are writing for a journal file, which is used for rollback and so on...
	// We just release the mutex and...
	ReleaseMutex(hMutex);
	// just use the ORIGINAL SQLite3 Write operation function.
	return winWrite(id,pBuf,amt,offset);
}

/*
   The function winReadEncrypted is the function that we developed to read from the SQLite3 Database Files
   using encryption instead of plain information. It takes 4 parameters:
   *id = the winFile Struct (described on comments above) that represent the journal file.
   *pBuf = the Buffer to write (a pointer to the start of the memory region containing the bytes)
   amt = the Number of bytes to write.
   offset = the position on the file where those bytes should be written.

   NOTICE THAT IS THE SAME SIGNATURE AS OUR winWriteEncrypted()
*/
int winReadEncrypted(
  void *id,               /* File to read into */
  const void *pBuf,               /* The bytes to read written */
  int amt,                        /* Number of bytes to read */
  __int64 offset            /* Offset into the file to begin writing at */
){
	// First, let us cast the pointer to its proper type...
	winFile *file = (winFile *)id;
	// First thing, we need of course to know what is the page that we are going to start reading.
	// So we get the offset and divide it by the size of each page, the int datatype will truncate
	// the remainder on the division, so no problem about that...
	int StartPage = offset / ENCRYPTION_PAGE_SIZE;
	// Later we need to calculate what is the last page that we will access, we get the startpage,
	// then we how many pages we need for the amt of bytes and we add a remainder just in case
	// that we offshoot the final page.
	int EndPage = StartPage + (amt / ENCRYPTION_PAGE_SIZE) + (amt % ENCRYPTION_PAGE_SIZE > 0 ? 1 : 0);
	// And we calculate the number of pages that we need to read.
	int NumberPagesToRead = (EndPage-StartPage);
	// The Size in bytes of the buffer that we need to store the data.
	int ReadBufferSize = NumberPagesToRead * ENCRYPTION_PAGE_SIZE;
	// Our Beloved Buffers, we will explain later...
	char *BufferToRead;
	char *bufferDecrypted;
	char *bufferEncrypted;
	int ret;

	// If we don't have a mutex let us create a mutex using
	// the proper SQLite3 native function.

	if(hMutex==NULL){
		hMutex = osCreateMutexW(NULL,FALSE,"E");
	}

	// If we have a mutex, let us wait for it in order to obtain its
	// ownership, just to avoid that 2 threads access the code below at the same time....
	WaitForSingleObject(hMutex,INFINITE);

	// We need to check if the name of the file that we are going to write is not a journal, 
	// those journals are rollback segments used for transactions within SQLite3, they exist
	// only for a few milliseconds and then are destroyed, we are only interested on the 
	// real database files.
	if(!strstr(file->zPath,"-journal")){
		// Ok, first thing, we need to allocate a buffer that is big enough to hold the
		// same size of the pBuf buffer that we need to store the data into.
		BufferToRead = malloc(ReadBufferSize);
		// We simply use the original SQLite3 original function to read the data from the
		// disk and store in BufferToRead.
		ret = winRead(id,(const void *)BufferToRead,ReadBufferSize,StartPage * ENCRYPTION_PAGE_SIZE);
		
		// We now allocate 2 more buffers, allocate twice the size just to make sure.
		bufferEncrypted = malloc(ReadBufferSize* 2);
		bufferDecrypted = malloc(ReadBufferSize* 2);
		// Copy the read buffer into BufferEncrypted...
		memcpy(bufferEncrypted,BufferToRead,ReadBufferSize);
		// Let us decrypt the buffer and place it in bufferDecrypted.
		DecryptPage(bufferDecrypted,bufferEncrypted, ReadBufferSize);
		// and then we copy the decrypted buffer into the final buffer that was 
		// passed by parameter.
		memcpy(pBuf,&bufferDecrypted[offset % ENCRYPTION_PAGE_SIZE],amt);
		// Let us deallocate our buffers..
		free(BufferToRead);
		free(bufferEncrypted);
		free(bufferDecrypted);
		// Now it is safe to release the mutex and the other threads can start
		// working
		ReleaseMutex(hMutex);
		// And return the returned value from the ReadOperation.
		return ret;
	}
	// Now it is safe to release the mutex and the other threads can start
	// working
	ReleaseMutex(hMutex);
	// If it is a  journal file, just pass the execution to the ORIGINAL SQLITE3 function.
	return winRead(id,pBuf,amt,offset);
}

// Our simple, but effective logging function...
void MVS_logToTextFile(const char *strToLog){
#ifdef LOGTEXTFILE_ENABLED
	FILE *test = fopen("log.txt","a");
	fprintf(test,"%s\r\n",strToLog);
	fclose(test);
#endif 
}

