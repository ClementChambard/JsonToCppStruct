#ifndef JSONREADER_H_
#define JSONREADER_H_

#include <json/json.h>
#include <vector>

struct JsonObjectField_t {
    JsonObjectField_t(std::string const& n, std::string const& t, int s, int o) :
        field_name(n), type_name(t), size(s), offset(o) {}
    std::string field_name;
    std::string type_name;
    int size;
    int offset;
};

struct JsonObjectFieldArray_t {
    JsonObjectFieldArray_t(std::string const& n, std::string const& t, int s, int o, int a) :
        field_name(n), type_name(t), single_size(s), first_offset(o), amount(a) {}
    std::string field_name;
    std::string type_name;
    int single_size;
    int first_offset;
    int amount;
};

struct JsonObjectType_t {
    JsonObjectType_t(std::string const& n, int s = 0) : name(n), size(s) {}
    std::string name;
    int size = 0;
    std::vector<JsonObjectField_t> fields;
    std::vector<JsonObjectFieldArray_t> array_fields;
};

class JsonReader {
    public:
        JsonReader(std::string const& filename, std::string const& main_object_type = "root");
        ~JsonReader();

        void* getObject();
        void writeObject(void* buf);
        void addObjectType(std::string const& name);
        void setObjectField(std::string const& object_type, std::string const& field_name, std::string const& field_type, int size = -1, int offset = -1, int ammount = 1);
        void setObjectField(std::string const& field_name, std::string const& field_type, int size = -1, int offset = -1, int ammount = 1);

        auto getJsonData() const { return completeJsonData; }

        static void cleanup();
    private:
        Json::Value completeJsonData;
        std::string filename;
        std::string main_object_name;
        std::string last_created_object;
        std::vector<JsonObjectType_t> object_types;
        uint8_t* out = nullptr;

        void decode_sub_object(std::string const& type_name, uint8_t* offset, Json::Value const& json);
        std::vector<JsonObjectType_t>::iterator get_type(std::string const& type_name);

        static std::map<std::string, char*> decoded_strings;
};

std::ostream& operator<<(std::ostream& l, JsonReader const& r);


#endif // JSONREADER_H_

// Macros for cleaner code
#ifdef  JSON_USE_MACROS
#ifndef JSON_MACROS_INCLUDED
#define JSON_MACROS_INCLUDED

#define JSON__OPEN(f) { auto json = new JsonReader(f);
#define JSON__TYPE(t) { t dtype; json->addObjectType(#t);
#define JSON__FIELD(f, t) json->setObjectField(#f, #t, sizeof(t), offsetof(typeof(dtype), f));
#define JSON__FIELDARR(f, t, amt) json->setObjectField(#f, #t, sizeof(t), offsetof(typeof(dtype), f), amt);
#define JSON__ENDTYPE }
#define JSON__ROOTFIELD(f, t, o, amt) json->setObjectField("root", #f, #t, sizeof(t), o, amt);
#define JSON__OUT(o) json->writeObject((void*)o);
#define JSON__CLOSE delete json; }
#define JSON__CLEANUP JsonReader::cleanup

#endif
#endif
