import json
import os
import argparse

type_map = {
    "str": "std::string",
    "bool": "bool",
    "int": "int",
    "float": "float",
    "double": "double",
    "ip": "IPAddress",
}



def generate_func_cast(type: str, key: str):
    if(type == "bool"):
        return f'this->{key} = (value == "1" || value == "true") ; this->__has_{key} = true'
    if(type == "enum"):
        return f'int opt; if(cast(value, opt)) {{ this->{key} = static_cast<{key}_Options>(opt); this->__has_{key} = true; }}'


    if type in ('float', 'double', 'int'):
        return f"if(cast(value, this->{key})) this->__has_{key} = true"

    return f"this->{key} = value; this->__has_{key} = true"

def generate_cast_to_string(type: str, key: str):
    if type == "enum":
        # return f"std::to_string(static_cast<int>(this->{key}))"
        return f"static_cast<int>(this->{key})"
    if type == "bool":
        # return f"std::string(this->{key} ? \"true\" : \"false\")"
        return f"this->{key}"
    if type == "ip":
        return f"this->{key}"
    if type == "str":
        return f"this->{key}"
    # return f"std::to_string(this->{key})"
    return f"this->{key}"

def convert_defaut_val(key: str, value: dict):
    val = value.get('default', '')
    type = value.get('type', 'str')
    if type == "str":
        return f'"{val}"'
    if type == "ip":
        return f'"{val}"'
    if type == "bool":
        return "true" if val else "false"
    if type == "enum":
        return f"{key}_Options::{val}"
    # if type in ('float', 'double', 'int'):
    #     return f"{type}({val})"
    # return val
    
    return f"{type}({val})"

def generate_cpp_code(config_file):
    with open(config_file, 'r') as file:
        config_data = json.load(file)

    config_name = os.path.splitext(os.path.basename(config_file))[0]
    config_name = config_name.replace('.', '_').replace('-', '_')
    config_name = config_name.capitalize() + '_KVConfig'
    struct_name = config_name
    struct_fields = []
    encode_lines = []
    decode_lines = []

    enum_fields = ""

    key_list = []
    for key, value in config_data.items():
        key_list.append(key)
        value_type = value.get("type", "str")
        
        field_type = ''
        if value_type == 'enum':
            enum_fields += f"enum class {key}_Options\n"
            enum_fields += "    {\n        "
            enum_fields += ",\n        ".join(value["options"])
            enum_fields += "\n    };\n"
            field_type = f"{key}_Options"
        else:
            field_type = type_map.get(value_type, "std::string")
        
        struct_fields.append(f"{field_type} {key};")
        encode_lines.append(f'oss << "{key}=" << {generate_cast_to_string(value_type, key)} << "\\n";')
        decode_lines.append(f'if (key == "{key}") {{ {generate_func_cast(value_type, key)}; return;}}')

    has_prop_fields_str = "\n    ".join([f"bool __has_{k};" for k,_ in config_data.items()])

    clean_has_prop_fields_str = "\n    ".join([f"this->__has_{k} = false;" for k,_ in config_data.items()])

    set_default_prop_str = "\n    ".join([f"if(!this->__has_{k}) this->{k} = {convert_defaut_val(k, value)};" for k, value in config_data.items()])

    struct_fields_str = "\n    ".join(struct_fields)
    encode_str = "\n        ".join(encode_lines)
    decode_str = "\n        ".join(decode_lines)

    cpp_code = f"""
#pragma once
#ifndef _KVGEN_{struct_name}_HPP
#define _KVGEN_{struct_name}_HPP
#include <string>
#include <sstream>
#include <kv_config/kv_config_base.hpp>

class {struct_name} : public KV_Config_Base
{{
public:
    {enum_fields}

    {struct_fields_str}

private:
    {has_prop_fields_str}
    
    void __encode(std::ostringstream &oss) {{
        {encode_str}
    }}

    void clear() {{
        {clean_has_prop_fields_str}
    }}

    void set_default_if_empty() {{
    
        {set_default_prop_str}
    }}

    void __update_config(const std::string& key, const std::string &value) {{
        {decode_str}
    }}

}};

#endif
"""
    return cpp_code


def main():
    parser = argparse.ArgumentParser(description='Generate C++ config header from JSON files.')
    parser.add_argument('--output_dir', type=str, required=True, help='Directory to store the generated header files.')
    parser.add_argument('json_files', type=str, nargs='+', help='List of JSON files to process.')
    args = parser.parse_args()

    output_dir = args.output_dir
    os.makedirs(output_dir, exist_ok=True)

    for json_file in args.json_files:
        cpp_code = generate_cpp_code(json_file)
        output_file = os.path.join(output_dir, os.path.splitext(os.path.basename(json_file))[0] + '.kv.hpp')
        with open(output_file, 'w') as file:
            file.write(cpp_code)
        print(f"Generated {output_file}")

if __name__ == "__main__":
    main()