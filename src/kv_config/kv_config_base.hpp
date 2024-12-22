#ifndef KV_CONFIG_BASE_HPP
#define KV_CONFIG_BASE_HPP

#include <string>
#include <sstream>
#include <array>
#include <cstdint>
#include <iostream>

class KV_Config_Base
{
public:
    virtual void Lock() {}
    virtual void UnLock() {}

    template <typename T>
    static bool cast(const std::string &value, T &out)
    {
        std::cout << value << std::endl;
        std::istringstream stream(value);
        stream >> out;
        stream >> std::ws;

        return !stream.fail() && stream.eof();
    }

    const std::string &str() const
    {
        return this->__config_str;
    }

    void encode();

    void Reset();

    void update_config(const std::string &key, const std::string &value)
    {
        this->Lock();
        this->__update_config(key, value);
        this->UnLock();
    }

    void update_config(const std::string &line)
    {
        this->Lock();
        this->__update_config(line);
        this->UnLock();
    }

    void decode(const std::string &config_str);

private:
    std::string __config_str;

    virtual void __encode(std::ostringstream &oss) = 0;
    virtual void __update_config(const std::string &key, const std::string &value) = 0;
    virtual void clear() = 0;
    virtual void set_default_if_empty() = 0;

    void __update_config(const std::string &line);
};

class IPAddress
{
    std::string ip;
    std::array<uint8_t, 4> addr;
    uint8_t mask;
    bool is_valid;

    void do_parse(std::istringstream &stream);

    void parse(const std::string &ipAddr);

    void encode();

public:
    IPAddress()
    {
        this->addr.fill(0);
        this->mask = 0;
        this->is_valid = false;
    }

    IPAddress(const char *ipAddr)
    {
        this->parse(ipAddr);
    }

    IPAddress(const std::string &ipAddr)
    {
        this->parse(ipAddr);
    }

    IPAddress(const std::array<uint8_t, 4> &addr, uint8_t mask = 0)
    {
        this->addr = addr;
        this->mask = mask;
        this->encode();
    }

    IPAddress(const IPAddress &ip)
    {
        this->ip = ip.ip;
        this->addr = ip.addr;
        this->mask = ip.mask;
    }

    bool IsDataValid() const
    {
        return this->is_valid;
    }

    const std::string &str() const
    {
        return this->ip;
    }

    const std::array<uint8_t, 4> &address() const
    {
        return this->addr;
    }

    uint8_t subnet() const
    {
        return this->mask ? this->mask : 24;
    }

    std::array<uint8_t, 4> sn_mask() const;
};

std::ostream &operator<<(std::ostream &os, const IPAddress &ip);

#endif // KV_CONFIG_BASE_HPP
