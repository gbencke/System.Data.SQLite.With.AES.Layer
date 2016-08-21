using System.Data.Entity;

namespace SQLite3EncryptionEFUnitTests.EFSchema
{
    public class SchoolConfiguration : DbConfiguration
    {
        public SchoolConfiguration()
        {
            SetDefaultConnectionFactory(new SchoolConnectionFactoryEf());
            SetDatabaseInitializer<SchoolContext>(null);
        }
    }
}
