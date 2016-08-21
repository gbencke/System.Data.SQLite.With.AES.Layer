#include <stdio.h>
#include <string.h>
#include <sqlite3.h> 
#include <xUnit++.h>

int GetNumbersOfRowsInTable(sqlite3 *db,char *tableName);
int GetNumbersOfStudentsWithGradeA(sqlite3 *db);
void DeleteDatabase(char *database);
sqlite3 *Sqlite3ReturnEncryptedDb(char *dbName);
int Sqlite3ExecWrapper(sqlite3 *db, char *sql);