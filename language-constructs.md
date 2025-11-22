# Object-Oriented Constructs

1. Classes
2. Functions and methods
3. Global Variables (Top level)
4. Typedefs

## Classes

Classes in Kahwa are just like in other programming languages such as Kotlin.

## Visibility Specifiers and `static`

We have three visibility specifiers `public`, `protected` and `private` with their usual meaning.
Specifically:

| Context                 | `public`                                     | `private`                       | `protected`                                     | `static`                                                                      |
|-------------------------|----------------------------------------------|---------------------------------|-------------------------------------------------|-------------------------------------------------------------------------------|
| **Top-level class**     | Visible everywhere in the project. (Default) | Private to the file.            | *Not allowed* for top-level classes.            | *Not allowed* for top-level classes.                                          |
| **Nested class**        | Visible everywhere in the project. (Default) | Private to the parent class.    | Visible to the parent class and its subclasses. | Has no effect. (Nested class = Static Nested in Java)                         |
| **Top-level functions** | Visible everywhere in the project. (Default) | Private to the file.            | *Not allowed* for top-level functions.          | *Not allowed* for top-level functions.                                        |
| **Methods**             | Visible everywhere in the project.           | Private to the class. (Default) | Visible to the parent class and its subclasses. | Equivalent to a top-level function but with visibility affected by the class. |
| **Global Variables**    | Visible everywhere in the project. (Default) | Private to the file.            | *Not allowed* for top-level variables.          | *Not allowed* for top-level variables.                                        |
| **Class fields**        | Visible everywhere in the project.           | Private to the class. (Default) | Visible to the parent class and its subclasses. | Equivalent to a top-level variable.                                           |
| **Typedefs**            | Visible everywhere in the project. (Default) | Private to the file.            | *Not allowed* for typedefs.                     | *Not allowed* for typedefs.                                                   |

### Class level privacy versus Object level privacy

Kahwa has class level privacy, which means that a method of a class can access private fields and methods of the instance of that class.

## Extending a class

Classes are final by default and can be marked as open for extending with the `open` keyword (just like Kotlin).
Similarly, if a class is open, then its methods and fields need to be marked as `open` in order to make them available for overriding.
An exception to this is that interface default methods are always `open` and marking them `open` doesn't do anything.

## Operators (WIP)

Kahwa has a single operator overload `()` for functions.
Any class can behave like a function, if they overload this operator.

