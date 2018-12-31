#pragma once
#ifndef _ASN_1_STREAM_
#define _ASN_1_STREAM_
#include <bitset>
#include <fstream>
#include <istream>
#include <streambuf>
#include <type_traits>
#include <vector>
#include <array>
namespace lwcp
{
typedef std::uint8_t byte;

typedef std::basic_streambuf<byte> byte_buffer;
typedef std::basic_fstream<byte> binary_fstream;
typedef std::basic_filebuf<byte> binary_fbuffer;

template <typename uint_t = uint8_t>
struct binary_value
{

    binary_value() = default;
    ~binary_value() = default;
    binary_value(uint_t _value) : octet(_value) {}

    template <uint_t bit>
    inline typename std::enable_if<((sizeof(uint_t) * 8) > bit)>::type clear_n()
    {
        octet &= ~(1U << bit);
    }

    template <uint_t bit, uint8_t value>
    inline typename std::enable_if<((sizeof(uint_t) * 8) > bit) &&
                                   (value == 0 || value == 1)>::type
    set_n()
    {
        uint_t newbit = !!value; // Also booleanize to force 0 or 1
        octet ^= (-newbit ^ octet) & (1UL << bit);
    }

    template <uint_t bit>
    inline typename std::enable_if<((sizeof(uint_t) * 8) > bit)>::type
    activate_n()
    {
        octet |= 1UL << bit;
    }

    template <uint_t bit>
    inline typename std::enable_if<((sizeof(uint_t) * 8) > bit), bool>::type
    is_active()
    {
        return (octet >> bit) & 1UL;
    }

    template <uint_t bit>
    inline typename std::enable_if<((sizeof(uint_t) * 8) > bit)>::type
    toggle_n()
    {
        octet ^= 1UL << bit;
    }

    inline uint_t get() { return octet; }

  private:
    uint_t octet;
};

struct data_value
{

    /*
   * bits 7 to 1 of the first subsequent octet, followed by bits 7 to 1 of the
   *second subsequent octet, followed in turn by bits 7 to 1 of each further
   *octet, up to and including the last subsequent octet in the identifier
   * octets shall be the encoding of an unsigned binary integer equal to the tag
   *number, with bit 7 of the first subsequent octet as the most significant
   *bit; c) bits 7 to 1 of the first subsequent octet shall not all be zero.
   */
    struct id
    {

        enum struct Class : std::uint8_t
        {
            Universal = 0,        // bytes 8-7=0-0,
            Application = 1,      // bytes 8-7=0-1,
            Context_specific = 2, // bytes 8-7=1-0,
            Private = 3           // bytes 8-7=1-1,
        };
        enum struct Encoding : std::uint8_t
        {
            Primitive = 0,
            Constructed = 1
        };

        id() = delete;
        ~id() = default;
        explicit id(byte b) : octet(b) {}

        inline Class get_class()
        {
            // xc0 == 11000000
            return static_cast<Class>((octet.to_ulong() & 0xc0) >> 6);
        }
        inline Encoding get_encoding()
        {
            // x20=00100000
            return static_cast<Encoding>((octet.to_ulong() & 0x20) >> 5);
        }
        inline uint8_t get_tag()
        {
            // implicit cast
            // 0x1f mask = 00011111 binary
            return (octet.to_ulong() & 0x1f);
        }
        inline bool has_subsequent_octets()
        {
            return get_tag() >= 0x1f; // >=31
        }
        inline bool is_last_sequence_octet() { return octet.test(7); }

      private:
        std::bitset<8> octet;
    };

    struct length
    {
    };
    struct content
    {
    };

    template <size_t size>
    inline void encode(std::array<byte, size> &buff)
    {
    }

    template <size_t size>
    inline void decode(std::streambuf &buff)
    {
        std::vector<id> ids;
        ids.emplace_back({buff.});
        if(ids[0].has_subsequent_octets())
        for (auto &_byte : buff)
        {
                id _id{_byte};
                ids.emplace_back(_id);
                if(_id.is_last_sequence_octet())
                break;

        }
    }
}; // namespace lwcp

// a) identifier octets (see 8.1.2);
// b) length octets (see 8.1.3);
// c) contents octets (see 8.1.4);
// d) end-of-contents octets (see 8.1.5).
}; // namespace lwcp

struct asn_1_stream
{
    asn_1_stream() = default;
    asn_1_stream(binary_fstream const &_input)
        : _binary_fbuffer(_input.rdbuf()) {}
    asn_1_stream(asn_1_stream &) = delete;

  private:
    binary_fbuffer _binary_fbuffer;
};

} // namespace lwcp

#endif