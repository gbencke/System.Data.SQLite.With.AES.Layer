#include "SQLite3EncryptedUnitTests.h"

int LastRowCount = -1;

static int callback(void *data, int argc, char **argv, char **azColName){
   LastRowCount = atoi(argv[0]);
   return 0;
}

void DeleteDatabase(char *database){
	char command[1000];
	sprintf(command,"del %s",database);
	system(command);
}

int GetNumbersOfStudentsWithGradeA(sqlite3 *db){
	int rc;
	char sql[1000];
	char *zErrMsg;
	const char* data = "Callback function called";

	sprintf(sql,"SELECT count(1) from STUDENT WHERE FinalGrade = \'A\' ");

    rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
		return -1;
    }else{
		return LastRowCount;
	}
}


int GetNumbersOfRowsInTable(sqlite3 *db,char *tableName){
	int rc;
	char sql[1000];
	char *zErrMsg;
	const char* data = "Callback function called";

	sprintf(sql,"SELECT count(1) from %s ",tableName);

    rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
		return -1;
    }else{
		return LastRowCount;
	}
}

int Sqlite3ExecWrapper(sqlite3 *db, char *sql){
    int  rc;
	char *zErrMsg;

	rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
	if(rc != SQLITE_OK){
		FILE *saida;
		saida = fopen("teste.txt","a");
		fprintf(saida,zErrMsg);
		fclose(saida);
	}
	return rc;
}

sqlite3 *Sqlite3ReturnEncryptedDb(char *dbName){
	char sql[1000];
	sqlite3 *db;
	int rc;

    rc = sqlite3_open(dbName, &db);
    if( rc ){
      return NULL;
    }

    strcpy(sql, " PRAGMA encryption_method=AES");
	rc = Sqlite3ExecWrapper(db,sql);
	if(!(rc==SQLITE_OK)){
		return NULL;
	}

    strcpy(sql, " PRAGMA encryption_keys=\"000102030405060708090a0b0c0d0e0f\"");
	rc = Sqlite3ExecWrapper(db,sql);
	if(!(rc==SQLITE_OK)){
		return NULL;
	}
	return db;
}

