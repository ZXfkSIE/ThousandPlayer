# Format manual

Although contributors should do their best to follow this manual, giving suggestions to this manual is always welcomed.

## If it is not referencing, do not `const` it

C++ provides a convenient qualifier `const` to prevent modification of an object.
Unfortunately,
too many `const`s will cause chaos in code since most objects will not be modified at all after their initialization.
In this project,
for the sake of readability,
`const` should only be used for reference types (including references and pointers) since local (stack) objects have little necessity of protection.

With regard to pointers,
since the main purpose of using `const` here is to protect the referenced object instead of the pointer itself,
it is adequate to write

```cpp
const QListWidgetItem *item1;
// Legal but not recommended for this project
QListWidgetItem const *item2;
``` 
rather than write
```cpp
// Only the pointer itself is protected
QListWidgetItem *const item3;
// It works, but... what an eyesore!
QListWidgetItem const*const item4;
```

## Put a line break between function's return type and definition

Borrowed from GNOME projects.

```cpp
int         // RETURN ↵
Class::getValue() const
{
    // ...
}
```

## Add prefix for function parameter names

- It is generally favorable to add the prefix `I_`, which means "input", to the names of function/method parameters.
- Sometimes it is also acceptable to add a type prefix to a variable name.

```cpp
void
Class::changeName( QString I_qstr_name )
{
    qstr_name = I_qstr_name;                    // "qstr_name" is a private member inside "Class"
    auto str_name { qstr_name.toStdString() };
    // ...
}
```

## Add spaces, indents and line breaks as you like

Do not hesitate to do everything you can to make the codes as readable as possible.
