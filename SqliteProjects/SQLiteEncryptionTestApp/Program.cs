using System;
using System.IO;

namespace SQLiteEncryptionTestApp
{
    class Program
    {
        private static void LogAndDataCleanUp()
        {
            if (File.Exists("test.db"))
                File.Delete("test.db");
            if (File.Exists("test_enc.db"))
                File.Delete("test_enc.db");

            if (File.Exists("test_ef.db"))
                File.Delete("test_ef.db");
            if (File.Exists("test_ef_enc.db"))
                File.Delete("test_ef_enc.db");

            if (File.Exists("log.txt"))
                File.Delete("log.txt");
        }

        private static void Banner(string Message)
        {
            Console.Out.WriteLine("==========================================================");
            Console.Out.WriteLine(Message);
            Console.Out.WriteLine("==========================================================");
        }

        static void Main(string[] args)
        {
            try
            {
                LogAndDataCleanUp();
                
                Banner("Testing Regular SQLite3 operations WITHOUT ENCRYPTION");
                SQLite3UnitTests.ExecuteTests(false, true);

                Banner("Testing Regular SQLite3 operations WITH ENCRYPTION");
                SQLite3UnitTests.ExecuteTests(true,true);
                
                Banner("Testing EF6 operations WITHOUT ENCRYPTION");
                SqLite3Ef6UnitTests.ExecuteTests(false);
                
                Banner("Testing EF6 operations WITH ENCRYPTION");
                SqLite3Ef6UnitTests.ExecuteTests(true);

            }
            catch (Exception ex)
            {
                Console.Out.WriteLine("Oooops, we have a issue:{0} - {1}",ex.Message,ex.StackTrace);
                Console.ReadLine();
                return;
            }
            Console.Out.WriteLine("Ok Everything seems to be ok...");
            Console.ReadLine();
        }

    }
}
