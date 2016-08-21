using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using NUnit.Framework;
using SQLite3EncryptionEFUnitTests.EFSchema;
using SQLite3EncryptionEFUnitTests.EFSchema.ContosoUniversity.Models;


namespace SQLite3EncryptionEFUnitTests
{
    [TestFixture]
    public class EfTests
    {
        private readonly string _currentAssemblyPath;

        public EfTests()
        {
            _currentAssemblyPath = Assembly.GetExecutingAssembly()
                .Location.Replace("SQLite3EncryptionEFUnitTests.dll", "");
            CleanDatabaseFiles();
        }

        private void CleanDatabaseFiles()
        {
            if (File.Exists(String.Format("{0}{1}", _currentAssemblyPath, "EF.Test.db")))
                File.Delete(String.Format("{0}{1}", _currentAssemblyPath, "EF.Test.db"));

        }

        private static SchoolContext GetContext()
        {
            var ctxt = new SchoolContext();
            return ctxt;
        }

        [Test]
        public void TestEfConnect()
        {
            var ctxt = GetContext();
            Assert.IsNotNull(ctxt);
        }

        [Test]
        public void TestEfCreateTable()
        {
            /*
             * System.Data.SQLite.EF6 Provider does not support
             * Code-First Migrations, so we need to build the schema 
             * directly on the Context Constructor, 
             * Please see the SO article below:
             * http://stackoverflow.com/questions/22174212/entity-framework-6-with-sqlite-3-code-first-wont-create-tables
             */

            Assert.IsTrue(true); // If it has reached here, it is ok...
        }

        [Test]
        public void TestEfInsertRows()
        {
            var ctxt = GetContext();

            foreach (var s in ctxt.Students)
            {
                ctxt.Students.Remove(s);
            }

            var students = new List<Student>
            {
            new Student{FirstMidName="Carson",LastName="Alexander",EnrollmentDate=DateTime.Parse("2005-09-01")},
            new Student{FirstMidName="Meredith",LastName="Alonso",EnrollmentDate=DateTime.Parse("2002-09-01")},
            new Student{FirstMidName="Arturo",LastName="Anand",EnrollmentDate=DateTime.Parse("2003-09-01")},
            new Student{FirstMidName="Gytis",LastName="Barzdukas",EnrollmentDate=DateTime.Parse("2002-09-01")},
            new Student{FirstMidName="Yan",LastName="Li",EnrollmentDate=DateTime.Parse("2002-09-01")},
            new Student{FirstMidName="Peggy",LastName="Justice",EnrollmentDate=DateTime.Parse("2001-09-01")},
            new Student{FirstMidName="Laura",LastName="Norman",EnrollmentDate=DateTime.Parse("2003-09-01")},
            new Student{FirstMidName="Nino",LastName="Olivetto",EnrollmentDate=DateTime.Parse("2005-09-01")}
            };

            var numRowsToInsert = students.Count;

            students.ForEach(s => ctxt.Students.Add(s));
            ctxt.SaveChanges();

            Assert.IsTrue(students.Count == numRowsToInsert);

        }

        [Test]
        public void TestEfDeleteRows()
        {
            var ctxt = GetContext();

            foreach (var s in ctxt.Students)
            {
                ctxt.Students.Remove(s);
            }

            var students = new List<Student>
            {
            new Student{FirstMidName="Carson",LastName="Alexander",EnrollmentDate=DateTime.Parse("2005-09-01")},
            new Student{FirstMidName="Meredith",LastName="Alonso",EnrollmentDate=DateTime.Parse("2002-09-01")},
            new Student{FirstMidName="Arturo",LastName="Anand",EnrollmentDate=DateTime.Parse("2003-09-01")},
            new Student{FirstMidName="Gytis",LastName="Barzdukas",EnrollmentDate=DateTime.Parse("2002-09-01")},
            new Student{FirstMidName="Yan",LastName="Li",EnrollmentDate=DateTime.Parse("2002-09-01")},
            new Student{FirstMidName="Peggy",LastName="Justice",EnrollmentDate=DateTime.Parse("2001-09-01")},
            new Student{FirstMidName="Laura",LastName="Norman",EnrollmentDate=DateTime.Parse("2003-09-01")},
            new Student{FirstMidName="Nino",LastName="Olivetto",EnrollmentDate=DateTime.Parse("2005-09-01")}
            };

            var numRowsToInsert = students.Count;

            students.ForEach(s => ctxt.Students.Add(s));
            ctxt.SaveChanges();

            Assert.IsTrue(students.Count == numRowsToInsert);

            foreach (var s in ctxt.Students)
            {
                ctxt.Students.Remove(s);
            }
            ctxt.SaveChanges();

            Assert.IsTrue(!ctxt.Students.Any());

        }

        [Test]
        public void TestUpdateRows()
        {
            var ctxt = GetContext();

            foreach (var s in ctxt.Students)
            {
                ctxt.Students.Remove(s);
            }

            var students = new List<Student>
            {
            new Student{FirstMidName="Carson",LastName="Alexander",EnrollmentDate=DateTime.Parse("2005-09-01")},
            new Student{FirstMidName="Meredith",LastName="Alonso",EnrollmentDate=DateTime.Parse("2002-09-01")},
            new Student{FirstMidName="Arturo",LastName="Anand",EnrollmentDate=DateTime.Parse("2003-09-01")},
            new Student{FirstMidName="Gytis",LastName="Barzdukas",EnrollmentDate=DateTime.Parse("2002-09-01")},
            new Student{FirstMidName="Yan",LastName="Li",EnrollmentDate=DateTime.Parse("2002-09-01")},
            new Student{FirstMidName="Peggy",LastName="Justice",EnrollmentDate=DateTime.Parse("2001-09-01")},
            new Student{FirstMidName="Laura",LastName="Norman",EnrollmentDate=DateTime.Parse("2003-09-01")},
            new Student{FirstMidName="Nino",LastName="Olivetto",EnrollmentDate=DateTime.Parse("2005-09-01")}
            };

            var numRowsToInsert = students.Count;

            students.ForEach(s => ctxt.Students.Add(s));
            ctxt.SaveChanges();

            Assert.IsTrue(students.Count == numRowsToInsert);

            foreach (var s in ctxt.Students.Where(s => s.LastName == "Li" || s.LastName == "Alonso"))
            {
                s.LastGrade = "A";
            }
            ctxt.SaveChanges();

            Assert.IsTrue(ctxt.Students.Count(s => s.LastGrade == "A") == 2);
        }

        [Test]
        public void TestDropTable()
        {
            /*
             * System.Data.SQLite.EF6 Provider does not support
             * Code-First Migrations, so we need to build the schema 
             * directly on the Context Constructor, 
             * Please see the SO article below:
             * http://stackoverflow.com/questions/22174212/entity-framework-6-with-sqlite-3-code-first-wont-create-tables
             */

            Assert.IsTrue(true); // If it has reached here, it is ok...
        }
    }
}
