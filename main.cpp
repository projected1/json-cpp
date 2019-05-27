/**
 * JSON reference:
 * http://json-schema.org/latest/json-schema-core.html#rfc.section.4.2.1
 *
 * null:    A JSON "null" production
 * boolean: A "true" or "false" value, from the JSON "true" or "false" productions
 * string:  A string of Unicode code points, from the JSON "string" production
 * number:  An arbitrary-precision, base-10 decimal number value, from the JSON "number" production
 * array:   An ordered list of instances, from the JSON "array" production
 * object:  An unordered set of properties mapping a string to an instance, from the JSON "object" production
 *
 * e.g.
 * { "a": 1, "b": [1, 2, "a"], "c": { "d": 2 } }
 *
 */
#include <array>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

class JsonValue {
public:
  virtual std::string toString() const = 0;
};

class JsonNull : public JsonValue {
public:
  std::string toString() const override {
    return "null";
  }
};

class JsonString : public JsonValue {
  std::string value_;
public:
  explicit JsonString(std::string value)
    : value_{ std::move(value) } {}

  std::string toString() const override {
    return "\"" + value_ + "\"";
  }
};

class JsonNumber : public JsonValue {
  double value_;
public:
  explicit JsonNumber(double value)
    : value_{ value } {}

  std::string toString() const override {
    std::stringstream ss;
    ss << value_;
    return ss.str();
  }
};

class JsonBoolean : public JsonValue {
  bool value_;
public:
  explicit JsonBoolean(bool value)
    : value_{ value } {}

  std::string toString() const override {
    std::stringstream ss;
    ss << std::boolalpha << value_;
    return ss.str();
  }
};

class JsonObject : public JsonValue {
  std::unordered_map<std::string, std::unique_ptr<JsonValue>> properties_;
public:
  void add(std::string name, std::unique_ptr<JsonValue> value) {
    properties_.insert({ std::move(name), std::move(value) });
  }

  std::string toString() const override {
    std::stringstream ss;
    ss << '{';
    for (const auto&[key, value] : properties_) {
      ss << key << ':' << value->toString() << ',';
    }
    ss << '}';
    return ss.str();
  }
};

class JsonArray : public JsonValue {
  std::vector<std::unique_ptr<JsonValue>> values_;
public:
  void add(std::unique_ptr<JsonValue> value) {
    values_.push_back(std::move(value));
  }

  std::string toString() const override {
    std::stringstream ss;
    ss << '[';
    for (const auto& value : values_) {
      ss << value->toString() << ',';
    }
    ss << ']';
    return ss.str();
  }
};

int main() {
  JsonObject root;
  root.add("a", std::make_unique<JsonString>("s1"));
  root.add("b", std::make_unique<JsonNumber>(1.1));
  root.add("c", std::make_unique<JsonBoolean>(true));
  root.add("d", std::make_unique<JsonBoolean>(false));
  root.add("e", std::make_unique<JsonNull>());

  auto obj = std::make_unique<JsonObject>();
  obj->add("f", std::make_unique<JsonString>("s2"));
  obj->add("g", std::make_unique<JsonNumber>(2.2));
  root.add("h", std::move(obj));

  auto arr = std::make_unique<JsonArray>();
  arr->add(std::make_unique<JsonString>("s3"));
  arr->add(std::make_unique<JsonNumber>(3.3));
  root.add("i", std::move(arr));

  // {i:["s3",3.3,],a:"s1",b:1.1,c:true,d:false,e:null,h:{f:"s2",g:2.2,},}
  std::cout << root.toString() << "\n";
}
