#include "SQLite3EncryptedUnitTests.h"

FACT("xUnit -> Teste Connect")
{
	 DeleteDatabase("xUnit.TestConnect.db");
	 sqlite3 *db = Sqlite3ReturnEncryptedDb("xUnit.TestConnect.db");
     Assert.NotEqual(0,(int)db);
}

FACT("xUnit -> Create Table")
{
	 int rc;
	 char sql[1000];

	 DeleteDatabase("xUnit.TestCreateTable.db");
	 sqlite3 *db = Sqlite3ReturnEncryptedDb("xUnit.TestCreateTable.db");
	 Assert.NotEqual(NULL,(int)db);
 
	 strcpy(sql, " CREATE TABLE STUDENT (");
     strcat(sql, "        ID INTEGER PRIMARY KEY AUTOINCREMENT, ");
     strcat(sql, "        LastName VARCHAR(100) ,");
     strcat(sql, "        FirstMidName VARCHAR(100) , ");
     strcat(sql, "        EnrollmentDate DATETIME, ");
	 strcat(sql, "        FinalGrade VARCHAR(1)) ");
	 rc = Sqlite3ExecWrapper(db,sql);
	 Assert.Equal(rc,SQLITE_OK);
	 sqlite3_close(db);
}

FACT("xUnit -> Insert Rows")
{
	 int rc;
	 char sql[1000];

	 DeleteDatabase("xUnit.TestInsertRows.db");
	 sqlite3 *db = Sqlite3ReturnEncryptedDb("xUnit.TestInsertRows.db");
     Assert.NotEqual(NULL,(int)db);

     strcpy(sql, " CREATE TABLE STUDENT (");
     strcat(sql, "        ID INTEGER PRIMARY KEY AUTOINCREMENT, ");
     strcat(sql, "        LastName VARCHAR(100) ,");
     strcat(sql, "        FirstMidName VARCHAR(100) , ");
     strcat(sql, "        EnrollmentDate DATETIME, ");
	 strcat(sql, "        FinalGrade VARCHAR(1)) ");
	 rc = Sqlite3ExecWrapper(db,sql);
	 Assert.Equal(rc,SQLITE_OK);

     strcpy(sql, " INSERT INTO STUDENT (LastName, FirstMidName,EnrollmentDate) values (\'Carson\',\'Alexander\',\'2015-06-01\')");
	 rc = Sqlite3ExecWrapper(db,sql);
	 Assert.Equal(rc,SQLITE_OK);

     strcpy(sql, " INSERT INTO STUDENT (LastName, FirstMidName,EnrollmentDate) values (\'Meredith\',\'Alonso\',\'2015-06-01\')");
	 rc = Sqlite3ExecWrapper(db,sql);
	 Assert.Equal(rc,SQLITE_OK);

     strcpy(sql, " INSERT INTO STUDENT (LastName, FirstMidName,EnrollmentDate) values (\'Arturo\',\'Anand\',\'2015-06-01\')");
	 rc = Sqlite3ExecWrapper(db,sql);
	 Assert.Equal(rc,SQLITE_OK);

     strcpy(sql, " INSERT INTO STUDENT (LastName, FirstMidName,EnrollmentDate) values (\'Yan\',\'Li\',\'2015-06-01\')");
	 rc = Sqlite3ExecWrapper(db,sql);
	 Assert.Equal(rc,SQLITE_OK);

	 int numRows = GetNumbersOfRowsInTable(db,"STUDENT");
	 Assert.Equal(4,numRows );

	 sqlite3_close(db);
}

FACT("xUnit -> Delete Rows")
{
	 int numRows;
	 int rc;
	 char sql[1000];

	 DeleteDatabase("xUnit.TestDeleteRows.db");
	 sqlite3 *db = Sqlite3ReturnEncryptedDb("xUnit.TestDeleteRows.db");
     Assert.NotEqual(NULL,(int)db);

     strcpy(sql, " CREATE TABLE STUDENT (");
     strcat(sql, "        ID INTEGER PRIMARY KEY AUTOINCREMENT, ");
     strcat(sql, "        LastName VARCHAR(100) ,");
     strcat(sql, "        FirstMidName VARCHAR(100) , ");
     strcat(sql, "        EnrollmentDate DATETIME, ");
	 strcat(sql, "        FinalGrade VARCHAR(1)) ");
	 rc = Sqlite3ExecWrapper(db,sql);
	 Assert.Equal(rc,SQLITE_OK);

     strcpy(sql, " INSERT INTO STUDENT (LastName, FirstMidName,EnrollmentDate) values (\'Carson\',\'Alexander\',\'2015-06-01\')");
	 rc = Sqlite3ExecWrapper(db,sql);
	 Assert.Equal(rc,SQLITE_OK);

     strcpy(sql, " INSERT INTO STUDENT (LastName, FirstMidName,EnrollmentDate) values (\'Meredith\',\'Alonso\',\'2015-06-01\')");
	 rc = Sqlite3ExecWrapper(db,sql);
	 Assert.Equal(rc,SQLITE_OK);

     strcpy(sql, " INSERT INTO STUDENT (LastName, FirstMidName,EnrollmentDate) values (\'Arturo\',\'Anand\',\'2015-06-01\')");
	 rc = Sqlite3ExecWrapper(db,sql);
	 Assert.Equal(rc,SQLITE_OK);

     strcpy(sql, " INSERT INTO STUDENT (LastName, FirstMidName,EnrollmentDate) values (\'Yan\',\'Li\',\'2015-06-01\')");
	 rc = Sqlite3ExecWrapper(db,sql);
	 Assert.Equal(rc,SQLITE_OK);

	 numRows = GetNumbersOfRowsInTable(db,"STUDENT");
	 Assert.Equal(4,numRows );

     strcpy(sql, " DELETE FROM STUDENT ");
	 rc = Sqlite3ExecWrapper(db,sql);
	 Assert.Equal(rc,SQLITE_OK);

	 numRows = GetNumbersOfRowsInTable(db,"STUDENT");
	 Assert.Equal(0,numRows );

	 sqlite3_close(db);
}

FACT("xUnit -> Update Rows")
{
	 int numRows;
	 int rc;
	 char sql[1000];

	 DeleteDatabase("xUnit.TestUpdateRows.db");
	 sqlite3 *db = Sqlite3ReturnEncryptedDb("xUnit.TestUpdateRows.db");
     Assert.NotEqual(NULL,(int)db);

     strcpy(sql, " CREATE TABLE STUDENT (");
     strcat(sql, "        ID INTEGER PRIMARY KEY AUTOINCREMENT, ");
     strcat(sql, "        LastName VARCHAR(100) ,");
     strcat(sql, "        FirstMidName VARCHAR(100) , ");
     strcat(sql, "        EnrollmentDate DATETIME, ");
	 strcat(sql, "        FinalGrade VARCHAR(1)) ");
	 rc = Sqlite3ExecWrapper(db,sql);
	 Assert.Equal(rc,SQLITE_OK);

	 strcpy(sql, " INSERT INTO STUDENT (LastName, FirstMidName,EnrollmentDate) values (\'Carson\',\'Alexander\',\'2015-06-01\')");
	 rc = Sqlite3ExecWrapper(db,sql);
	 Assert.Equal(rc,SQLITE_OK);

     strcpy(sql, " INSERT INTO STUDENT (LastName, FirstMidName,EnrollmentDate) values (\'Meredith\',\'Alonso\',\'2015-06-01\')");
	 rc = Sqlite3ExecWrapper(db,sql);
	 Assert.Equal(rc,SQLITE_OK);

     strcpy(sql, " INSERT INTO STUDENT (LastName, FirstMidName,EnrollmentDate) values (\'Arturo\',\'Anand\',\'2015-06-01\')");
	 rc = Sqlite3ExecWrapper(db,sql);
	 Assert.Equal(rc,SQLITE_OK);

     strcpy(sql, " INSERT INTO STUDENT (LastName, FirstMidName,EnrollmentDate) values (\'Yan\',\'Li\',\'2015-06-01\')");
	 rc = Sqlite3ExecWrapper(db,sql);
	 Assert.Equal(rc,SQLITE_OK);

	 numRows = GetNumbersOfRowsInTable(db,"STUDENT");
	 Assert.Equal(4,numRows );

     strcpy(sql, " UPDATE STUDENT SET FinalGrade = \'A\' WHERE LASTNAME IN (\'Arturo\',\'Yan\')");
	 rc = Sqlite3ExecWrapper(db,sql);
	 Assert.Equal(rc,SQLITE_OK);

	 numRows = GetNumbersOfStudentsWithGradeA(db);
	 Assert.Equal(2,numRows );

	 sqlite3_close(db);

}

FACT("xUnit -> Drop Table")
{
	 int rc;
	 char sql[1000];

	 DeleteDatabase("xUnit.TestDropTable.db");
	 sqlite3 *db = Sqlite3ReturnEncryptedDb("xUnit.TestDropTable.db");
	 Assert.NotEqual(NULL,(int)db);

     strcpy(sql, " CREATE TABLE STUDENT (");
     strcat(sql, "        ID INTEGER PRIMARY KEY AUTOINCREMENT, ");
     strcat(sql, "        LastName VARCHAR(100) ,");
     strcat(sql, "        FirstMidName VARCHAR(100) , ");
     strcat(sql, "        EnrollmentDate DATETIME, ");
	 strcat(sql, "        FinalGrade VARCHAR(1)) ");
	 rc = Sqlite3ExecWrapper(db,sql);
	 Assert.Equal(rc,SQLITE_OK);

     strcpy(sql, " DROP TABLE STUDENT ");
	 rc = Sqlite3ExecWrapper(db,sql);
	 Assert.Equal(rc,SQLITE_OK);

	 sqlite3_close(db);
}
