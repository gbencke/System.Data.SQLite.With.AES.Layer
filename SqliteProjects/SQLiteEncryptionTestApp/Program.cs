using System;
using System.Collections.Generic;
using System.Data;
using System.Data.Common;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SQLiteEncryptionTestApp
{
    class Program
    {
        static void Main(string[] args)
        {
            const string fileName = "test.db";
            var prov = "";

            using (var tbl = DbProviderFactories.GetFactoryClasses())
            {
                foreach (DataRow row in tbl.Rows)
                {
                    prov = row[2].ToString();

                    if (prov.IndexOf("SQLite", 0, StringComparison.OrdinalIgnoreCase) == -1 &&
                        prov.IndexOf("SqlClient", 0, StringComparison.OrdinalIgnoreCase) == -1) continue;
                    if (prov == "System.Data.SQLite") break;
                }
            }

            if (String.IsNullOrEmpty(prov))
            {
                throw new ApplicationException("SQLite Provider not found! Check App.config file");
            }

            var factory = DbProviderFactories.GetFactory(prov);
            var cnnString = String.Format("Data Source={0};Pooling=true;FailIfMissing=false", fileName);

            var cnn = factory.CreateConnection();
            if (cnn == null)
            {
                throw new ApplicationException("Failure to create connection");
            }
            cnn.ConnectionString = cnnString;
            
            var cnnstring = factory.CreateConnectionStringBuilder();
            if (cnnstring == null)
            {
                throw new ApplicationException("Failure to CreateConnectionStringBuilder");
            }
            cnnstring.ConnectionString = cnnString;
            cnn.Open();

            using (var cmd = cnn.CreateCommand())
            {
                if (factory.GetType().Name.IndexOf("SQLite", StringComparison.OrdinalIgnoreCase) == -1)
                    cmd.CommandText = "CREATE TABLE TestCase (ID bigint primary key identity, Field1 integer, Field2 Float, [Fiëld3] VARCHAR(50), [Fiæld4] CHAR(10), Field5 DateTime, Field6 Image)";
                else
                    cmd.CommandText = "CREATE TABLE TestCase (ID integer primary key autoincrement, Field1 int, Field2 Float, [Fiëld3] VARCHAR(50), [Fiæld4] CHAR(10), Field5 DateTime, Field6 Image)";

                cmd.ExecuteNonQuery();
            }


        }
    }
}
