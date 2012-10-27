#ifndef JSONFACTORY_HPP_
#define JSONFACTORY_HPP_

#include <string>
#include <stdexcept>

#include <pficommon/text/json.h>

namespace jsonfactory {

class UnknownType : public std::exception {
 public:
  UnknownType(const std::string& type)
      : type_(type),
        msg_("Unknown type: \"" + type + "\"") {}
  ~UnknownType() throw() {}

  const char* what() const throw() { return msg_.c_str(); }
 private:
  const std::string type_;
  const std::string msg_;
};

class DuplicatedType : public std::exception {
 public:
  DuplicatedType(const std::string& type)
      : type_(type),
        msg_("Duplicated type: \"" + type + "\"") {}
  ~DuplicatedType() throw() {}

  const char* what() const throw() { return msg_.c_str(); }
 private:
  const std::string type_;
  const std::string msg_;
};

////////

template <typename T>
class Factory {
 public:
  typedef pfi::lang::shared_ptr<Factory> Ptr;

  virtual ~Factory() {}
  virtual T* create() const = 0;
};

template <typename T, typename C, typename P>
class FactoryWithParameter : public Factory<T> {
 public:
  FactoryWithParameter(const P& param)
      : param_(param) {}
  
  T* create() const {
    return new C(param_);
  }
 private:
  const P param_;
};

template <typename T, typename C>
class FactoryWithoutParameter : public Factory<T> {
 public:
  T* create() const {
    return new C;
  }
};

////////

template <typename T>
class FactoryManager {
 public:
  template <typename C, typename P>
  void register_class(const std::string& type,
                      void (*check)(const P&) = 0) {
    check_type(type);
    generators_[type] = GeneratorPtr(new GeneratorWithParameter<C, P>(check));
  }

  template <typename C>
  void register_without_parameter(const std::string& type) {
    check_type(type);
    generators_[type] = GeneratorPtr(new GeneratorWithoutParameter<C>);
  }

  T* create(const std::string& type, const pfi::text::json::json& param) {
    return create_generator(type, param)->create();
  }

  typename Factory<T>::Ptr create_generator(const std::string& type,
                                            const pfi::text::json::json& param) {
    if (generators_.count(type) > 0)
      return generators_[type]->gen(param);
    else
      throw UnknownType(type);
  }

 private:
  class Generator {
   public:
    virtual ~Generator() {}
    virtual typename Factory<T>::Ptr gen(const pfi::text::json::json& param) const = 0;
  };

  template <typename C, typename P>
  class GeneratorWithParameter : public Generator {
   public:
    GeneratorWithParameter(void (*check)(const P&) = 0) : check_fun_(check) {}

    typename Factory<T>::Ptr gen(const pfi::text::json::json& param) const {
      P p = pfi::text::json::json_cast<P>(param);
      if (check_fun_)
        (*check_fun_)(p);
      return typename Factory<T>::Ptr(new FactoryWithParameter<T, C, P>(p));
    }

   private:
    void (*check_fun_) (const P&);
  };

  template <typename C>
  class GeneratorWithoutParameter : public Generator {
   public:
    typename Factory<T>::Ptr gen(const pfi::text::json::json&) const {
      return typename Factory<T>::Ptr(new FactoryWithoutParameter<T, C>);
    }
  };

  typedef pfi::lang::shared_ptr<Generator> GeneratorPtr;

  void check_type(const std::string& type) const {
    if (generators_.count(type) > 0) {
      throw DuplicatedType(type);
    }
  }

  std::map<std::string, GeneratorPtr> generators_;
};

} // jsonfactory

#endif // JSONFACTORY_HPP_
