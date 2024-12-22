#include "test.kv.hpp"
#include <iostream>

void test_decode()
{
    std::istringstream iss("   enum  abc ");
    std::string key, value;

    iss >> key;
    iss >> value;

    std::cout << key << "-" << value << std::endl;

    Test_KVConfig config;
    config.decode("str_cfg=hello\nbool_cfg=ags\nint_cfg=42\nfloat_cfg=7.14\ndouble_cfg=7.14159\nip_cfg=");

    std::cout << config.str_cfg << std::endl;

    config.encode();
    std::cout << config.str() << std::endl;

    std::cout << std::endl;
    config.decode("str_cfg=hello\nbool_cfg=ags\nint_cfg=42\nfloat_cfg=7.14\ndouble_cfg=7.14159\nip_cfg=192.168.1.1");

    config.encode();
    std::cout << config.str() << std::endl;

    std::cout << std::endl;
    config.decode("  enum_cfg  = 2 \n str_cfg=hello world \nbool_cfg=ags\nint_cfg=42\nfloat_cfg=7.14\ndouble_cfg=7.14159\nip_cfg=192.168.1.1");

    config.encode();
    std::cout << config.str() << std::endl;
}

int main()
{
    test_decode();

    return 0;

    Test_KVConfig config;
    config.str_cfg = "hello";
    config.bool_cfg = true;
    config.int_cfg = 42;
    config.float_cfg = 3.14f;
    config.double_cfg = 3.14159;
    config.ip_cfg = "127.0.0.1";
    config.enum_cfg = Test_KVConfig::enum_cfg_Options::enum2;

    config.encode();
    std::string config_str = config.str();

    std::cout << config_str << std::endl;

    config.decode(config_str);

    // int x;
    // bool ok = Test_Config::cast("12 \t ", x);

    // std::cout << x << " " << ok << std::endl;

    // std::string value = "";
    // std::ostringstream oss(value);
    // oss << 12;

    // std::cout << oss.str() << std::endl;
    // std::cout << value << std::endl;

    IPAddress ip = ("127.0.0.1");
    IPAddress ip2 = ip;
    IPAddress ip3 = std::move(ip2);
    // IPAddress ip("127.0.0.1/");
    // IPAddress ip(".127.0.0.1");
    // IPAddress ip("127.0.0.1");
    // IPAddress ip("127.0.0.1/24");
    std::ostringstream oss;
    oss << ip3;
    std::cout << oss.str() << std::endl;

    // std::cout << static_cast<std::string>(ip3) << std::endl;

    return 0;
}