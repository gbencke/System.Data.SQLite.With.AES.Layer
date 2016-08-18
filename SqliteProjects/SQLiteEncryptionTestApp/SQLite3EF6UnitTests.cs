using System;
using System.Collections.Generic;
using System.Data.Entity;
using SQLiteEncryptionTestApp.ContosoUniversity.Models;

namespace SQLiteEncryptionTestApp
{
    public class SqLite3Ef6UnitTests
    {
        protected static void Seed(SchoolContext context)
        {
            foreach (var s in context.Students)
            {
                context.Students.Remove(s);
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

            students.ForEach(s => context.Students.Add(s));
            context.SaveChanges();

        }


        public static void ExecuteTests(bool shouldEncrypt)
        {

            SchoolConnectionFactory.ShouldEncrypt = shouldEncrypt;//TOOD:Move this to a config file, please....
            SchoolConnectionFactory.ResetConfigDB();

            var db = new SchoolContext();

            Seed(db);

            foreach (var student in db.Students)
            {
                Console.Out.WriteLine("Student:{0}",student.LastName);
            }

        }
    }
}
