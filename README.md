jsonfactory
===========

What's this
-----------

A simple implementation of factory object using templates.
It has these features:

- Create concrete instance with JSON configuration
- To check type safety, cast the JSON configuration to your own struct
- Check configuration with an arbitrary function

Requirement
-----------

- c++
- pficommon


Usage
-----

Prepare interface class, concrete classes, their parameter class and constructors using the parameters.
The parameter must be json serializable (use pficommon's serializer).

```c++
class Interface { ... };

struct YourParameter {
  template <typename Ar>
  void serialize(Ar& ar) { ... }
};

class YourClass : public Interface {
 public:
   YourClass(const YourParameter& parameter) { ... }
...
};
```

Make a FactoryManager of the interface.
And, then register your classes with an arbitrary name, that is used when you create an instance.

```c++
FactoryManager<Interface> f;
f.register_class<YourClass, YourParameter>("you_class_name");
f.register_class<AnotherClass, AnotherParameter>("another_class_name"); 
```

To create an instance, call `create` metod with the name of the instance and pficommon's json object.
It returns a raw pointer of a created instance with `new`.

```c++
pfi::text::json::json param; // <- read from file
Interface* inst(f.create("your_class_name", param));
```

If the class need no parameter, register it with `register_without_parameter`.

```c++
f.register_without_parameter<ClassWithoutParameter>("class_name");
```

To check the parameter take a check function to `register_class`.

```c++
void check_fun(const YourParameter& p) {
  if (p.name >= 32) {
    throw std::runtime_error("Name must be lesser than 32");
  }
}

...

f.register_class<YourClass, YourParameter>("checked_class", &check_fun)
```

If you only want to check a parameter, and want to create actual instance afterward, call `create_factory` method.
This method cast a json to a parameter, call check function and make a factory.
Call `create` method of the factory to create an actual instance.

```c++
Factory<Interface>::Ptr g(f.create_factory("your_class_name", param));
// ...
Interface* inst = g->create();
```

This method is usefull when you want to check all configuration of several obejcts.


License
-------

This library is distributed under MIT license.
