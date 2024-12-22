#include "kv_config/kv_config_base.hpp"

void KV_Config_Base::decode(const std::string &config_str)
{

    std::istringstream stream(config_str);
    std::string line;

    this->Lock();
    this->clear();

    stream >> std::ws;

    while (std::getline(stream, line))
    {
        std::istringstream line_stream(line);
        std::string key, value;

        if(!std::getline(line_stream, key, '='))
        {
            continue;
        }

        stream >> std::ws;

        if(!std::getline(line_stream, value))
        {
            continue;
        }

        key.erase(key.find_last_not_of(" \t\n\r\f\v") + 1);
        value.erase(value.find_last_not_of(" \t\n\r\f\v") + 1);
        this->__update_config(key, value);

        stream >> std::ws;
    }

    this->set_default_if_empty();

    this->UnLock();
}

void KV_Config_Base::encode()
{
    this->Lock();

    std::ostringstream oss;

    this->__encode(oss);

    this->__config_str = oss.str();

    this->UnLock();
}

void KV_Config_Base::Reset()
{
    this->Lock();
    this->clear();
    this->set_default_if_empty();
    this->UnLock();
}

void KV_Config_Base::__update_config(const std::string &line)
{

    std::istringstream line_stream(line);
    std::string key, value;
    if (std::getline(line_stream, key, '=') && std::getline(line_stream, value))
    {
        this->__update_config(key, value);
    }
}

std::ostream &operator<<(std::ostream &os, const IPAddress &ip)
{
    os << ip.str();
    return os;
}

void IPAddress::do_parse(std::istringstream &stream)
{
    std::string octet;
    std::array<unsigned, 4> tmp;

    this->is_valid = true;

    for (unsigned i = 0; i < 4; i++)
    {
        stream >> tmp[i];
        if (stream.fail())
        {
            this->is_valid = false;
            break;
        }

        if (tmp[i] > 255)
        {
            this->is_valid = false;
            break;
        }

        if (i < 3)
        {
            char delim = '\0';
            stream >> delim;
            if (delim != '.')
            {
                this->is_valid = false;
                break;
            }
        }
    }

    if (!this->is_valid)
    {
        return;
    }

    std::copy(tmp.begin(), tmp.end(), this->addr.begin());

    if (stream.eof())
    {
        this->mask = 0;
        return;
    }

    char delim = '\0';
    stream >> delim;

    if (delim != '/')
    {
        this->is_valid = false;
        return;
    }

    unsigned tmpMask = 0;
    stream >> tmpMask;
    this->mask = tmpMask > 32 ? 0 : tmpMask;

    if (!this->mask)
    {
        this->is_valid = false;
    }
}

void IPAddress::parse(const std::string &ipAddr)
{
    std::istringstream stream(ipAddr);
    std::string octet;

    this->do_parse(stream);

    if (this->is_valid)
    {
        this->encode();
    }
    else
    {
        this->ip = "";
    }
}

void IPAddress::encode()
{
    std::ostringstream oss;
    oss << (int)this->addr[0] << "."
        << (int)this->addr[1] << "."
        << (int)this->addr[2] << "."
        << (int)this->addr[3];

    if (this->mask)
    {
        oss << "/" << (int)this->mask;
    }

    this->ip = oss.str();
}

std::array<uint8_t, 4> IPAddress::sn_mask() const
{
    uint8_t sn_num = this->subnet();
    std::array<uint8_t, 4> sn;

    uint32_t tmp = 0xFFFFFFFF;
    for (unsigned i = 32; i > sn_num; i--)
    {
        tmp <<= 1;
    }

    uint8_t *pTmp = (uint8_t *)&tmp;
    sn[0] = pTmp[3];
    sn[1] = pTmp[2];
    sn[2] = pTmp[1];
    sn[3] = pTmp[0];

    return sn;
}