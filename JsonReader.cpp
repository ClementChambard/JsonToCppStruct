#include "JsonReader.h"

#include <cstddef>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstring>

std::map<std::string, char*> JsonReader::decoded_strings;

JsonReader::JsonReader(std::string const& filename, std::string const& main_object_name)
{
    this->filename = filename;
    this->main_object_name = main_object_name;
    addObjectType(main_object_name);

    std::ifstream file(filename);
    Json::Reader reader;

    reader.parse(file, completeJsonData);

    file.close();
}

JsonReader::~JsonReader()
{
    if (out) delete[] out;
}

void JsonReader::cleanup()
{
    for (auto p : decoded_strings) free(p.second);
}

void* JsonReader::getObject()
{
    if (out) delete[] out;
    out = new uint8_t[get_type(main_object_name)->size];
    decode_sub_object(main_object_name, out, completeJsonData);
    return out;
}

void JsonReader::writeObject(void* out)
{
    decode_sub_object(main_object_name, (uint8_t*) out, completeJsonData);
}

void JsonReader::addObjectType(std::string const& name)
{
    object_types.push_back(JsonObjectType_t{name});
    last_created_object = name;
}

void JsonReader::setObjectField(std::string const& field_name, std::string const& field_type, int size, int offset, int amount) { setObjectField(last_created_object, field_name, field_type, size, offset, amount); }

void JsonReader::setObjectField(std::string const& object_type, std::string const& field_name, std::string const& field_type, int size, int offset, int amount)
{
    if (amount == 0) return;
    auto obj = get_type(object_type);
    if (offset == -1) offset = obj->size;
    if (size == -1) size = get_type(field_type)->size;
    if (amount > 1) obj->array_fields.push_back(JsonObjectFieldArray_t{field_name, field_type, size, offset, amount});
    else obj->fields.push_back(JsonObjectField_t{field_name, field_type, size, offset});
    if (offset + size * amount > obj->size) obj->size = offset + size * amount;
}

std::vector<JsonObjectType_t>::iterator JsonReader::get_type(std::string const& name)
{
    return std::find_if(object_types.begin(), object_types.end(), [name](JsonObjectType_t& o){ return o.name == name; });
}

void JsonReader::decode_sub_object(std::string const& obj_type, uint8_t* offset, Json::Value const& json)
{
    // put built in types here
    if (obj_type == "int")     { *(int    *)offset = json.asInt();   return; }
    if (obj_type == "int32_t") { *(int32_t*)offset = json.asInt();   return; }
    if (obj_type == "float")   { *(float  *)offset = json.asFloat(); return; }
    if (obj_type == "char*")
    {
        std::string val = json.asString();
        auto p = decoded_strings.find(val);
        if (p == decoded_strings.end())
        {
            char* buf = strdup(val.c_str());
            decoded_strings.insert({val, buf});
            *(char**)offset = buf;
        }
        else *(char**)offset = p->second;
        return;
    }

    for (auto f : get_type(obj_type)->fields) decode_sub_object(f.type_name, offset + f.offset, json[f.field_name]);
    for (auto a : get_type(obj_type)->array_fields)
    {
        for (int i = 0; i < a.amount; i++) {
            decode_sub_object(a.type_name, offset + a.first_offset + i * a.single_size, json[a.field_name][i]);
        }
    }
}

std::ostream& operator<<(std::ostream& l, JsonReader const& r)
{
    l << r.getJsonData();
    return l;
}
