# Format manual

Although contributors should do their best to follow this manual, giving suggestions to this manual is always welcomed.

## If it is not referencing, do not `const` it

C++ provides a convenient qualifier `const` to protect the modification of an object.
Unfortunately, too many `const`s will cause chaos in code since most objects will not be modified at all after their initialization.
In this project,
for the sake of readability,
`const` should only be used for reference types (including references and pointers) since local (stack) objects have little necessity of protection.

With regard to pointers,
since the primary purpose of using `const` here is to protect the referenced object instead of the pointer itself,
it is adequate to write

```cpp
const QListWidgetItem *item1 { /*...*/ };
// Legal but not recommended for this project
QListWidgetItem const *item2 { /*...*/ };
``` 
rather than write
```cpp
// What an eyesore!
QListWidgetItem const*const item1;
```

## Put a line break between function's return type and definition

Borrowed from GNOME projects.

```cpp
int
Class::getValue() const
{
    // ...
}
```

## Add prefix for function parameter names

- It is generally favorable to add the prefix `I_`, which means "input", to names of function/method parameters.
- Sometimes it is also acceptable to add a type prefix to variable names.

```cpp
void
Class::changeName( QString I_qstr_Name )
{
    qstr_Name = I_qstr_Name;                // "qstr_Name" is a private member
    auto str_name { qstr_name.toStdString() };
    // ...
}
```

## Add spaces, indents and line breaks as you like

Do not hesitate to do everything you can to make the codes as readable as possible.