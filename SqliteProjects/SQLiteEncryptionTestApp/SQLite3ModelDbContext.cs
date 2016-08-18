
using System;
using System.Data.Common;
using System.Data.Entity;
using System.Data.Entity.Infrastructure;
using System.Data.Entity.ModelConfiguration.Conventions;
using System.Text;
using SQLiteEncryptionTestApp.ContosoUniversity.Models;

namespace SQLiteEncryptionTestApp
{
    public class SchoolConnectionFactory : IDbConnectionFactory
    {
        public static bool ShouldEncrypt = false;

        private static readonly Object LockObject = new Object();
        private static bool _initialized;
        private  String _currentSqlConnectionString;
        private  String _databaseName;

        public static void ResetConfigDB()
        {
            _initialized = false;
        }

        private void ResetConnectionInfo()
        {
            _databaseName = ShouldEncrypt ? "test_ef_enc.db" : "test_ef.db";
            _currentSqlConnectionString = String.Format("Data Source={0};Pooling=true;FailIfMissing=false", _databaseName);
            
        }
        public SchoolConnectionFactory()
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

            if (ShouldEncrypt)
            {
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
            }

            using (var cmd = cnn.CreateCommand())
            {
                var sb = new StringBuilder();
                sb.Append("CREATE TABLE STUDENT (");
                sb.Append("ID INTEGER PRIMARY KEY AUTOINCREMENT, ");
                sb.Append("LastName VARCHAR(100) ,");
                sb.Append("FirstMidName VARCHAR(100) , ");
                sb.Append("EnrollmentDate DATETIME) ");
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

    public class SchoolConfiguration : DbConfiguration
    {
        public SchoolConfiguration()
        {
            SetDefaultConnectionFactory(new SchoolConnectionFactory());
            SetDatabaseInitializer<SchoolContext>(null);
            
        }
    } 

    public class SchoolContext : DbContext
    {

        public SchoolContext()
        {
            Console.Out.WriteLine("SchoolContext");
        }

        public DbSet<Student> Students { get; set; }
        public DbSet<Enrollment> Enrollments { get; set; }
        public DbSet<Course> Courses { get; set; }

        protected override void OnModelCreating(DbModelBuilder modelBuilder)
        {
            modelBuilder.Conventions.Remove<PluralizingTableNameConvention>();
        }
    }

}
