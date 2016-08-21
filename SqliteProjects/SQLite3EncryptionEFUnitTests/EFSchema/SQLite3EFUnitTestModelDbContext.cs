using System.Data.Entity;
using System.Data.Entity.ModelConfiguration.Conventions;
using SQLite3EncryptionEFUnitTests.EFSchema.ContosoUniversity.Models;

namespace SQLite3EncryptionEFUnitTests.EFSchema
{
    public class SchoolContext : DbContext
    {

        public DbSet<Student> Students { get; set; }

        protected override void OnModelCreating(DbModelBuilder modelBuilder)
        {
            modelBuilder.Conventions.Remove<PluralizingTableNameConvention>();
        }
    }
}
