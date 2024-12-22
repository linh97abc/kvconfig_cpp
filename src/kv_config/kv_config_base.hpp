#ifndef KV_CONFIG_BASE_HPP
#define KV_CONFIG_BASE_HPP

#include <string>
#include <sstream>
#include <array>
#include <cstdint>
#include <iostream>


/**
 * @class KV_Config_Base
 * @brief A base class for key-value configuration management.
 *
 * This class provides a base interface for managing key-value configurations.
 * It includes methods for locking and unlocking the configuration, encoding
 * and decoding configuration data, and updating the configuration with key-value
 * pairs or lines. The exact encoding, updating, and clearing mechanisms are
 * determined by the derived classes.
 *
 * @note This class is intended to be subclassed, and certain methods must be
 *       implemented by the derived classes.
 */
class KV_Config_Base
{
public:
    virtual void Lock() {}
    virtual void UnLock() {}

    template <typename T>
    static bool cast(const std::string &value, T &out)
    {
        // std::cout << value << std::endl;
        std::istringstream stream(value);
        stream >> out;

        if (!stream.eof())
        {
            stream >> std::ws;
        }

        return !stream.fail() && stream.eof();
    }

    const std::string &str() const
    {
        return this->__config_str;
    }

    /**
     * @brief Encodes the current configuration data.
     *
     * This function is responsible for encoding the configuration data
     * into a specific format. The exact encoding mechanism and format
     * are determined by the implementation.
     */
    void encode();

    /**
     * @brief Resets the configuration to its default state.
     */
    void Reset();

    /**
     * @brief Updates the configuration with the given key-value pair.
     *
     * This function trims leading and trailing whitespace from both the key and value,
     * then locks the configuration, updates it with the provided key-value pair, and
     * finally unlocks the configuration.
     *
     * @param key The configuration key to update.
     * @param value The new value for the configuration key.
     */
    void update_config(const std::string &key, const std::string &value);

    /**
     * @brief Updates the configuration with the provided line.
     *
     * This function locks the configuration, updates it with the given line,
     * and then unlocks the configuration. It ensures that the update operation
     * is thread-safe.
     *
     * @param line The configuration line to update.
     */
    void update_config(const std::string &line)
    {
        this->Lock();
        this->__update_config(line);
        this->UnLock();
    }

    /**
     * @brief Decodes the given configuration string.
     *
     * This function takes a configuration string in a specific format and decodes it
     * to extract the configuration parameters.
     *
     * @param config_str The configuration string to decode.
     */
    void decode(const std::string &config_str);

private:
    std::string __config_str;

    virtual void __encode(std::ostringstream &oss) = 0;
    virtual void __update_config(const std::string &key, const std::string &value) = 0;
    virtual void clear() = 0;
    virtual void set_default_if_empty() = 0;

    void __update_config(const std::string &line);
};

/**
 * @class IPAddress
 * @brief A class to represent and manipulate IP addresses.
 * 
 * This class provides functionalities to parse, encode, and validate IP addresses.
 * It supports initialization from strings, character arrays, and byte arrays.
 */
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
