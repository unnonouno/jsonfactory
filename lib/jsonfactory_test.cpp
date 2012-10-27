#include <gtest/gtest.h>
#include "jsonfactory.hpp"

#include <pficommon/lang/scoped_ptr.h>

using namespace std;
using namespace pfi::text::json;
using pfi::lang::scoped_ptr;

namespace jsonfactory {

class Interface {
 public:
  virtual ~Interface() {}
  virtual string get() const = 0;
};

class Class1 : public Interface {
 public:
  string get() const { return "class1"; }
};

class Class2 : public Interface {
 public:
  Class2(const string& s) : str_(s) {}
  string get() const { return str_; }
 private:
  string str_;
};

TEST(FactoryManager, without_parameter) {
  FactoryManager<Interface> m;
  m.register_without_parameter<Class1>("class1");
  {
    scoped_ptr<Interface> p(m.create("class1", json()));
    EXPECT_EQ("class1", p->get());
  }
  {
    Factory<Interface>::Ptr f(m.create_generator("class1", json()));
    scoped_ptr<Interface> p(f->create());
    EXPECT_EQ("class1", p->get());
  }
}

TEST(FactoryManager, with_parameter) {
  FactoryManager<Interface> m;
  m.register_class<Class2, string>("class2");
  json param(new json_string("saitama"));
  {
    scoped_ptr<Interface> p(m.create("class2", param));
    EXPECT_EQ("saitama", p->get());
  }
  {
    Factory<Interface>::Ptr f(m.create_generator("class2", param));
    scoped_ptr<Interface> p(f->create());
    EXPECT_EQ("saitama", p->get());
  }
}

TEST(FactoryManager, duplicated_type) {
  FactoryManager<Interface> m;
  m.register_without_parameter<Class1>("class1");
  EXPECT_THROW(m.register_without_parameter<Class1>("class1"), DuplicatedType);
  EXPECT_THROW((m.register_class<Class2, string>("class1")), DuplicatedType);
}

TEST(FactoryManager, unknown_type) {
  FactoryManager<Interface> m;
  EXPECT_THROW(m.create("unknow_type_name", json()), UnknownType);
}


TEST(FactoryManager, duplicated_class) {
}

TEST(FactoryManager, bad_cast) {
  FactoryManager<Interface> m;
  m.register_class<Class2, string>("class2");
  json param(new json_integer(1));
  EXPECT_THROW(m.create("class2", param), bad_cast);
  EXPECT_THROW(m.create_generator("class2", param), bad_cast);
}

}
