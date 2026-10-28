# Student Management System

A console-based **Student Management System** developed using C++ and MySQL to manage student academic records, provide student information access, and generate branch-wise CGPA rankings.

## Features

### Admin Portal
- Admin signup and login.
- Add new student records.
- View and search student details using Scholar ID.
- Update and delete student records.
- Generate branch-wise CGPA rankings in descending order.

### Student Portal
- Student login using Scholar ID.
- View personal details, including name, branch, CGPA, and email.
- Check branch-wise academic rankings.

## Tech Stack

- **Programming Language:** C++
- **Database:** MySQL
- **Connectivity:** MySQL C API
- **Compiler:** GCC / MinGW
- **IDE:** Visual Studio Code

## Database Structure

The project uses a MySQL database named `student_management` with two tables:

- **Students:** Stores Scholar ID, name, branch, CGPA, and email.
- **Admins:** Stores administrator usernames and Caesar-encrypted passwords.

Supported branches include EI, CSE, ECE, and EE.

## Concepts Used

- Object-Oriented Programming (OOP)
- MySQL database connectivity
- SQL CRUD operations
- Console-based menu-driven programming
- Sorting and ranking using SQL queries
- Basic authentication and Caesar cipher encryption

## Future Improvements

- Implement secure password hashing and stronger authentication.
- Use prepared statements for database operations.
- Develop a graphical or web-based interface.
- Add advanced student performance analytics and reports.

## Note

This project is intended for educational purposes. Caesar cipher encryption is used for learning and is not suitable for real-world password security.
