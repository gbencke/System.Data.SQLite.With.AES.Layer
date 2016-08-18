

namespace SQLiteEncryptionTestApp
{
    class Program
    {

        static void Main(string[] args)
        {
            SQLite3UnitTests.ExecuteTests();
            SQLite3EF6UnitTests.ExecuteTests(null);
        }

    }
}
