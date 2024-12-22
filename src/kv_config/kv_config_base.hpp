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


    /**
     * @brief Locks the configuration to prevent concurrent modifications.
     * 
     * This method is intended to be overridden by derived classes to implement
     * specific locking mechanisms. The default implementation does nothing.
     */
    virtual void Lock() {}

    /**
     * @brief Unlocks the configuration to allow modifications.
     * 
     * This method is intended to be overridden by derived classes to implement
     * specific unlocking mechanisms. The default implementation does nothing.
     */
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

    /**
     * @brief Returns the configuration data as a string.
     *
     * This function returns the configuration data as a string.
     *
     * @return const std::string& The configuration data as a string.
     */
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
    /**
     * @brief Default constructor for the IPAddress class.
     * 
     * This constructor initializes the IPAddress object with default values:
     * - The address is set to all zeros.
     * - The mask is set to 0.
     * - The validity flag is set to false.
     */
    IPAddress()
    {
        this->addr.fill(0);
        this->mask = 0;
        this->is_valid = false;
    }

    /**
     * @brief Constructs an IPAddress object and parses the given IP address string.
     * 
     * @param ipAddr A C-style string representing the IP address to be parsed.
     */
    IPAddress(const char *ipAddr)
    {
        this->parse(ipAddr);
    }


    /**
     * @brief Constructs an IPAddress object and parses the given IP address string.
     * 
     * @param ipAddr A std::string representing the IP address to be parsed.
     */
    IPAddress(const std::string &ipAddr)
    {
        this->parse(ipAddr);
    }

    /**
     * @brief Constructs an IPAddress object with the given address and optional mask.
     * 
     * @param addr A std::array of 4 uint8_t elements representing the IP address.
     * @param mask An optional uint8_t value representing the subnet mask. Default is 0.
     */
    IPAddress(const std::array<uint8_t, 4> &addr, uint8_t mask = 0)
    {
        this->addr = addr;
        this->mask = mask;
        this->encode();
    }

    /**
     * @brief Copy constructor for the IPAddress class.
     *
     * This constructor creates a new IPAddress object by copying the values
     * from an existing IPAddress object.
     *
     * @param ip The IPAddress object to copy from.
     */
    IPAddress(const IPAddress &ip)
    {
        this->ip = ip.ip;
        this->addr = ip.addr;
        this->mask = ip.mask;
    }

    /**
     * @brief Checks if the data is valid.
     *
     * @return true if the data is valid, false otherwise.
     */
    bool IsDataValid() const
    {
        return this->is_valid;
    }

    /**
     * @brief Returns the IP address as a string.
     * 
     * This function provides access to the IP address stored in the object.
     * 
     * @return const std::string& A reference to the IP address string.
     */
    const std::string &str() const
    {
        return this->ip;
    }

    /**
     * @brief Get the address as a constant reference to a std::array of 4 uint8_t elements.
     *
     * @return const std::array<uint8_t, 4>& Reference to the address array.
     */
    const std::array<uint8_t, 4> &address() const
    {
        return this->addr;
    }

    /**
     * @brief Retrieves the subnet mask.
     *
     * This function returns the subnet mask. If the mask is not set (i.e., it is zero),
     * it defaults to returning 24.
     *
     * @return uint8_t The subnet mask, or 24 if the mask is not set.
     */
    uint8_t subnet() const
    {
        return this->mask ? this->mask : 24;
    }

    /**
     * @brief Calculates the subnet mask based on the subnet number.
     *
     * This function computes the subnet mask for the IP address by shifting bits
     * according to the subnet number. The subnet mask is returned as an array of
     * four 8-bit unsigned integers.
     *
     * @return std::array<uint8_t, 4> The subnet mask as an array of four bytes.
     */
    std::array<uint8_t, 4> sn_mask() const;
};

/**
 * @brief Overloaded output stream operator for the IPAddress class.
 *
 * This function overloads the output stream operator to allow printing
 * IPAddress objects to an output stream.
 *
 * @param os The output stream to write to.
 * @param ip The IPAddress object to write.
 * @return std::ostream& A reference to the output stream.
 */
std::ostream &operator<<(std::ostream &os, const IPAddress &ip);

#endif // KV_CONFIG_BASE_HPP
