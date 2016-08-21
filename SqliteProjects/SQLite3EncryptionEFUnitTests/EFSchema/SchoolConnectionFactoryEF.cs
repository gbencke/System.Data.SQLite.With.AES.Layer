using System;
using System.Data.Common;
using System.Data.Entity.Infrastructure;
using System.Reflection;
using System.Text;

namespace SQLite3EncryptionEFUnitTests.EFSchema
{
    public class SchoolConnectionFactoryEf : IDbConnectionFactory
    {
        private static readonly Object LockObject = new Object();
        private static bool _initialized;
        private String _currentSqlConnectionString;

        public static void ResetConfigDb()
        {
            _initialized = false;
        }

        private void ResetConnectionInfo()
        {
            var currentAssemblyPath = Assembly.GetExecutingAssembly()
                .Location.Replace("SQLite3EncryptionEFUnitTests.dll", "");

            _currentSqlConnectionString =
                String.Format("Data Source={0}EF.Test.db;Pooling=true;FailIfMissing=false", 
                currentAssemblyPath);
        }
        public SchoolConnectionFactoryEf()
        {
            ResetConnectionInfo();
        }

        private static void CreateSqLiteDatabase(string connString)
        {
            var fact = DbProviderFactories.GetFactory("System.Data.SQLite");

            var cnn = fact.CreateConnection();
            if (cnn == null)
            {
                throw new ApplicationException("Failure to create connection");
            }
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
     
            using (var cmd = cnn.CreateCommand())
            {
                var sb = new StringBuilder();
                sb.Append("CREATE TABLE STUDENT (");
                sb.Append("ID INTEGER PRIMARY KEY AUTOINCREMENT, ");
                sb.Append("LastName VARCHAR(100) ,");
                sb.Append("FirstMidName VARCHAR(100) , ");
                sb.Append("EnrollmentDate DATETIME, ");
                sb.Append("LastGrade VARCHAR(1)) ");
                cmd.CommandText = sb.ToString();
                cmd.ExecuteNonQuery();
            }

            cnn.Close();

        }

        public DbConnection CreateConnection(string nameOrConnectionString)
        {

            lock (LockObject)
            {
                if (!_initialized)
                {
                    ResetConnectionInfo();
                    CreateSqLiteDatabase(_currentSqlConnectionString);
                    _initialized = true;
                }
            }

            var factory = DbProviderFactories.GetFactory("System.Data.SQLite.EF6");
            var dbConnection = factory.CreateConnection();

            if (dbConnection != null)
            {
                dbConnection.ConnectionString = _currentSqlConnectionString;
                return dbConnection;
            }
            else
            {
                throw new ApplicationException("Error in creating connection on ConnectionFactory");
            }
        }

    }
}
