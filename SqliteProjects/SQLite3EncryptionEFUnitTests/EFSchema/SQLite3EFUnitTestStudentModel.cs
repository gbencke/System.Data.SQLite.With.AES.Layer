using System;

namespace SQLite3EncryptionEFUnitTests.EFSchema
{
    namespace ContosoUniversity.Models
    {
        public enum Grade
        {
            A, B, C, D, F
        }

        public class Student
        {
            public int ID { get; set; }
            public string LastName { get; set; }
            public string FirstMidName { get; set; }
            public DateTime EnrollmentDate { get; set; }
            public string LastGrade { get; set; }

        }

    }
}
