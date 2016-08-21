using System;
using System.Text;
using System.IO;
using System.Data.Common;
using System.Reflection;
using NUnit.Framework;


namespace SQLite3EncryptionADOUnitTests
{
    [TestFixture]
    public class MainTests
    {
        private readonly string _currentAssemblyPath;

        public MainTests()
        {
            _currentAssemblyPath = Assembly.GetExecutingAssembly().Location.Replace("SQLite3EncryptionADOUnitTests.dll", "");
            CleanDatabaseFiles();
        }

        private void CleanDatabaseFiles()
        {
            if (File.Exists(String.Format("{0}{1}", _currentAssemblyPath,"ADO.TestConnection.db")))
                File.Delete(String.Format("{0}{1}", _currentAssemblyPath, "ADO.TestConnection.db"));

            if (File.Exists(String.Format("{0}{1}", _currentAssemblyPath, "ADO.TestCreateTable.db")))
                File.Delete(String.Format("{0}{1}", _currentAssemblyPath, "ADO.TestCreateTable.db"));

            if (File.Exists(String.Format("{0}{1}", _currentAssemblyPath, "ADO.TestInsertRow.db")))
                File.Delete(String.Format("{0}{1}", _currentAssemblyPath, "ADO.TestInsertRow.db"));

            if (File.Exists(String.Format("{0}{1}", _currentAssemblyPath, "ADO.TestDeleteRow.db")))
                File.Delete(String.Format("{0}{1}", _currentAssemblyPath, "ADO.TestDeleteRow.db"));

            if (File.Exists(String.Format("{0}{1}", _currentAssemblyPath, "ADO.TestUpdateRow.db")))
                File.Delete(String.Format("{0}{1}", _currentAssemblyPath, "ADO.TestUpdateRow.db"));

            if (File.Exists(String.Format("{0}{1}", _currentAssemblyPath, "ADO.TestDropTable.db")))
                File.Delete(String.Format("{0}{1}", _currentAssemblyPath, "ADO.TestDropTable.db"));


        }

        private DbConnection GetConnection(string databaseName)
        {
            var fact = DbProviderFactories.GetFactory("System.Data.SQLite");

            var cnn = fact.CreateConnection();
            if (cnn == null)
            {
                throw new ApplicationException("Failure to create connection");
            }

            var connString = String.Format("Data Source={0}{1};Pooling=true;FailIfMissing=false", _currentAssemblyPath, databaseName);
            cnn.ConnectionString = connString;
            cnn.Open();

            using (var cmd = cnn.CreateCommand())
            {
                cmd.CommandText = "PRAGMA encryption_method=AES";
                cmd.ExecuteNonQuery();
            }

            using (var cmd = cnn.CreateCommand())
            {
                cmd.CommandText = "PRAGMA encryption_keys=\"000102030405060708090a0b0c0d0e0f\"";
                cmd.ExecuteNonQuery();
            }

            return cnn;

        }


        private static int GetNumbersOfStudentsWithGradeA(DbConnection cnn)
        {
            var ret = -1;

            using (var cmd = cnn.CreateCommand())
            {
                cmd.CommandText = String.Format("SELECT COUNT(1) FROM STUDENT WHERE FinalGrade = \'A\' ");
                var dr = cmd.ExecuteReader();
                if (dr.Read())
                {
                    ret = Int32.Parse(dr[0].ToString());
                }
            }

            return ret;
        }


        private static int GetNumbersOfRowsInTable(DbConnection cnn, string tableName)
        {
            var ret = -1;

            using (var cmd = cnn.CreateCommand())
            {
                cmd.CommandText = String.Format("SELECT COUNT(1) FROM {0}", tableName);
                var dr = cmd.ExecuteReader();
                if (dr.Read())
                {
                    ret = Int32.Parse(dr[0].ToString());
                }
            }

            return ret;
        }

        [Test]
        public void TestConnect()
        {
            var cnn = GetConnection("ADO.TestConnection.db");
            Assert.IsNotNull(cnn);
        }
        [Test]
        public void TestCreateTable()
        {
            var cnn = GetConnection("ADO.TestCreateTable.db");
            Assert.IsNotNull(cnn);
            using (var cmd = cnn.CreateCommand())
            {
                var sb = new StringBuilder();
                sb.Append("CREATE TABLE STUDENT (");
                sb.Append("ID INTEGER PRIMARY KEY AUTOINCREMENT, ");
                sb.Append("LastName VARCHAR(100) ,");
                sb.Append("FirstMidName VARCHAR(100) , ");
                sb.Append("EnrollmentDate DATETIME, ");
                sb.Append("FinalGrade VARCHAR(1)) ");
                cmd.CommandText = sb.ToString();
                cmd.ExecuteNonQuery();
            }
            Assert.IsTrue(true); // If it has reached here, it is ok...
        }
        [Test]
        public void TestInsertRows()
        {
            var cnn = GetConnection("ADO.TestInsertRow.db");
            Assert.IsNotNull(cnn);
            using (var cmd = cnn.CreateCommand())
            {
                var sb = new StringBuilder();
                sb.Append("CREATE TABLE STUDENT (");
                sb.Append("ID INTEGER PRIMARY KEY AUTOINCREMENT, ");
                sb.Append("LastName VARCHAR(100) ,");
                sb.Append("FirstMidName VARCHAR(100) , ");
                sb.Append("EnrollmentDate DATETIME, ");
                sb.Append("FinalGrade VARCHAR(1)) ");
                cmd.CommandText = sb.ToString();
                cmd.ExecuteNonQuery();
            }

            using (var cmd = cnn.CreateCommand())
            {
                var sb = new StringBuilder();
                sb.Append(" INSERT INTO STUDENT (LastName, FirstMidName,EnrollmentDate) values (\'Carson\',\'Alexander\',\'2015-06-01\')");
                cmd.CommandText = sb.ToString();
                cmd.ExecuteNonQuery();
            }

            using (var cmd = cnn.CreateCommand())
            {
                var sb = new StringBuilder();
                sb.Append(" INSERT INTO STUDENT (LastName, FirstMidName,EnrollmentDate) values (\'Meredith\',\'Alonso\',\'2015-06-01\')");
                cmd.CommandText = sb.ToString();
                cmd.ExecuteNonQuery();
            }

            using (var cmd = cnn.CreateCommand())
            {
                var sb = new StringBuilder();
                sb.Append(" INSERT INTO STUDENT (LastName, FirstMidName,EnrollmentDate) values (\'Arturo\',\'Anand\',\'2015-06-01\')");
                cmd.CommandText = sb.ToString();
                cmd.ExecuteNonQuery();
            }

            using (var cmd = cnn.CreateCommand())
            {
                var sb = new StringBuilder();
                sb.Append(" INSERT INTO STUDENT (LastName, FirstMidName,EnrollmentDate) values (\'Yan\',\'Li\',\'2015-06-01\')");
                cmd.CommandText = sb.ToString();
                cmd.ExecuteNonQuery();
            }

            var numRows = GetNumbersOfRowsInTable(cnn, "STUDENT");
            Assert.IsTrue(4 == numRows);

        }
        [Test]
        public void TestDeleteRows()
        {
            var cnn = GetConnection("ADO.TestDeleteRow.db");
            Assert.IsNotNull(cnn);
            using (var cmd = cnn.CreateCommand())
            {
                var sb = new StringBuilder();
                sb.Append("CREATE TABLE STUDENT (");
                sb.Append("ID INTEGER PRIMARY KEY AUTOINCREMENT, ");
                sb.Append("LastName VARCHAR(100) ,");
                sb.Append("FirstMidName VARCHAR(100) , ");
                sb.Append("EnrollmentDate DATETIME, ");
                sb.Append("FinalGrade VARCHAR(1)) ");
                cmd.CommandText = sb.ToString();
                cmd.ExecuteNonQuery();
            }

            using (var cmd = cnn.CreateCommand())
            {
                var sb = new StringBuilder();
                sb.Append(" INSERT INTO STUDENT (LastName, FirstMidName,EnrollmentDate) values (\'Carson\',\'Alexander\',\'2015-06-01\')");
                cmd.CommandText = sb.ToString();
                cmd.ExecuteNonQuery();
            }

            using (var cmd = cnn.CreateCommand())
            {
                var sb = new StringBuilder();
                sb.Append(" INSERT INTO STUDENT (LastName, FirstMidName,EnrollmentDate) values (\'Meredith\',\'Alonso\',\'2015-06-01\')");
                cmd.CommandText = sb.ToString();
                cmd.ExecuteNonQuery();
            }

            using (var cmd = cnn.CreateCommand())
            {
                var sb = new StringBuilder();
                sb.Append(" INSERT INTO STUDENT (LastName, FirstMidName,EnrollmentDate) values (\'Arturo\',\'Anand\',\'2015-06-01\')");
                cmd.CommandText = sb.ToString();
                cmd.ExecuteNonQuery();
            }

            using (var cmd = cnn.CreateCommand())
            {
                var sb = new StringBuilder();
                sb.Append(" INSERT INTO STUDENT (LastName, FirstMidName,EnrollmentDate) values (\'Yan\',\'Li\',\'2015-06-01\')");
                cmd.CommandText = sb.ToString();
                cmd.ExecuteNonQuery();
            }

            var numRows = GetNumbersOfRowsInTable(cnn, "STUDENT");
            Assert.IsTrue(4 == numRows);

            using (var cmd = cnn.CreateCommand())
            {
                var sb = new StringBuilder();
                sb.Append(" DELETE FROM STUDENT;");
                cmd.CommandText = sb.ToString();
                cmd.ExecuteNonQuery();
            }

            numRows = GetNumbersOfRowsInTable(cnn, "STUDENT");
            Assert.IsTrue(0 == numRows);
        }
        
        [Test]
        public void TestUpdateRows()
        {
            var cnn = GetConnection("ADO.TestUpdateRow.db");
            Assert.IsNotNull(cnn);
            using (var cmd = cnn.CreateCommand())
            {
                var sb = new StringBuilder();
                sb.Append("CREATE TABLE STUDENT (");
                sb.Append("ID INTEGER PRIMARY KEY AUTOINCREMENT, ");
                sb.Append("LastName VARCHAR(100) ,");
                sb.Append("FirstMidName VARCHAR(100) , ");
                sb.Append("EnrollmentDate DATETIME, ");
                sb.Append("FinalGrade VARCHAR(1)) ");
                cmd.CommandText = sb.ToString();
                cmd.ExecuteNonQuery();
            }

            using (var cmd = cnn.CreateCommand())
            {
                var sb = new StringBuilder();
                sb.Append(" INSERT INTO STUDENT (LastName, FirstMidName,EnrollmentDate) values (\'Carson\',\'Alexander\',\'2015-06-01\')");
                cmd.CommandText = sb.ToString();
                cmd.ExecuteNonQuery();
            }

            using (var cmd = cnn.CreateCommand())
            {
                var sb = new StringBuilder();
                sb.Append(" INSERT INTO STUDENT (LastName, FirstMidName,EnrollmentDate) values (\'Meredith\',\'Alonso\',\'2015-06-01\')");
                cmd.CommandText = sb.ToString();
                cmd.ExecuteNonQuery();
            }

            using (var cmd = cnn.CreateCommand())
            {
                var sb = new StringBuilder();
                sb.Append(" INSERT INTO STUDENT (LastName, FirstMidName,EnrollmentDate) values (\'Arturo\',\'Anand\',\'2015-06-01\')");
                cmd.CommandText = sb.ToString();
                cmd.ExecuteNonQuery();
            }

            using (var cmd = cnn.CreateCommand())
            {
                var sb = new StringBuilder();
                sb.Append(" INSERT INTO STUDENT (LastName, FirstMidName,EnrollmentDate) values (\'Yan\',\'Li\',\'2015-06-01\')");
                cmd.CommandText = sb.ToString();
                cmd.ExecuteNonQuery();
            }

            var numRows = GetNumbersOfRowsInTable(cnn, "STUDENT");
            Assert.IsTrue(4 == numRows);

            using (var cmd = cnn.CreateCommand())
            {
                var sb = new StringBuilder();
                sb.Append("UPDATE STUDENT SET FinalGrade = \'A\' WHERE LASTNAME IN (\'Arturo\',\'Yan\')");
                cmd.CommandText = sb.ToString();
                cmd.ExecuteNonQuery();
            }

            numRows = GetNumbersOfStudentsWithGradeA(cnn);
            Assert.IsTrue(2 == numRows);
        }
        [Test]
        public void TestDropTable()
        {
            var cnn = GetConnection("ADO.TestDropTable.db");
            Assert.IsNotNull(cnn);
            using (var cmd = cnn.CreateCommand())
            {
                var sb = new StringBuilder();
                sb.Append("CREATE TABLE STUDENT (");
                sb.Append("ID INTEGER PRIMARY KEY AUTOINCREMENT, ");
                sb.Append("LastName VARCHAR(100) ,");
                sb.Append("FirstMidName VARCHAR(100) , ");
                sb.Append("EnrollmentDate DATETIME, ");
                sb.Append("FinalGrade VARCHAR(1)) ");
                cmd.CommandText = sb.ToString();
                cmd.ExecuteNonQuery();
            }

            using (var cmd = cnn.CreateCommand())
            {
                var sb = new StringBuilder();
                sb.Append(" DROP TABLE STUDENT;");
                cmd.CommandText = sb.ToString();
                cmd.ExecuteNonQuery();
            }

            Assert.IsTrue(true); // If it has reached here, it is ok...
        }
    }
}
