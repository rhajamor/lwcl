#pragma once
#ifndef _ASN_1_STREAM_
#define _ASN_1_STREAM_
#include <algorithm>
#include <array>
#include <bitset>
#include <fstream>
#include <istream>
#include <iterator>
#include <streambuf>
#include <type_traits>
#include <vector>
#include <initializer_list>

#if _HAS_CXX17
#include <any>
#endif
namespace lwcp
{

#if !(_HAS_CXX17 || _HAS_STD_BYTE)
	// ENUM CLASS byte_t
	enum class byte_t : std::uint8_t
	{
	};
#else
	using byte_t = std::byte;
#endif
	//
	// typedef std::basic_iostream<byte> byte_buffer;
	// typedef std::basic_fstream<byte> binary_fstream;
	// typedef std::basic_filebuf<byte> binary_fbuffer;
	namespace details
	{
		template<typename T>
		constexpr size_t compute_len(size_t n) {
			return n;
		}
	}
	template <const uint32_t nBits, typename uint_t = typename std::enable_if<
		(nBits > 0) && (nBits <= sizeof(std::uintmax_t) * 8),
		typename std::conditional<nBits <= sizeof(std::uint8_t) * 8, std::uint8_t,
		typename std::conditional<nBits <= sizeof(std::uint16_t) * 8, std::uint16_t,
		typename std::conditional<nBits <= sizeof(std::uint32_t) * 8, std::uint32_t, std::uint64_t>::type>::type>::type>::type
>
struct bitset_t
	{

		bitset_t() = default;
		~bitset_t() = default;
		bitset_t(uint_t _value = 0) : _bits(_value) {}
		bitset_t(std::bitset<nBits> const &_value) : _bits(_value.to_ulong()) {}
		bitset_t(std::string const &_value) : bitset_t(std::bitset<nBits>(_value)) {}

		template <uint32_t bit>
		inline typename std::enable_if<((sizeof(uint_t) * 8) > bit)>::type clear_n()
		{
			_bits &= ~(1UL << bit);
		}

		template <uint32_t bit, std::uint8_t value>
		inline typename std::enable_if<((sizeof(uint_t) * 8) > bit) &&
			(value == 0 || value == 1)>::type
			set_n()
		{
			bool newbit = !!value; // Also booleanize to force 0 or 1
			_bits ^= (-newbit ^ _bits) & (1UL << bit);
		}

		template <uint32_t bit>
		inline typename std::enable_if<((sizeof(uint_t) * 8) > bit)>::type
			activate_n()
		{
			_bits |= 1UL << bit;
		}

		template <uint32_t bit>
		inline typename std::enable_if<((sizeof(uint_t) * 8) > bit), bool>::type
			is_active()
		{
			return (_bits >> bit) & 1UL;
		}

		template <uint32_t bit>
		inline typename std::enable_if<((sizeof(uint_t) * 8) > bit)>::type
			toggle_n()
		{
			_bits ^= 1UL << bit;
		}

		inline const uint_t get() { return _bits; }

		inline std::string to_string()
		{
			return std::bitset<nBits>(_bits).to_string();
		}

		constexpr const size_t size()
		{
			return nBits;
		}

	private:
		uint_t _bits;
	};
	template <class _bitset_in, class _bitset_out >
	inline _bitset_out join(_bitset_in const *_iter_beg, _bitset_in const *_iter_end, size_t start_bit = 0, size_t end_bit = std::string::npos)
	{
		std::string byte_string;
		std::for_each(_iter_beg, _iter_end, [&byte_string](_bitset_in &set) {
			byte_string.append(set.to_string().substr(start_bit, end_bit));
		});
		std::static_assert(byte_string.size() < sizeof(std::uintmax_t) * 8, "integer overflow");
		std::move(_bitset_out<byte_string.size()>(byte_string));
	}
	struct data_value
	{

		struct Id
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

			Id() = delete;
			~Id() = default;
			explicit Id(byte_t b) : octet{ static_cast<std::uint8_t>(b) } {}

			inline Class get_class()
			{
				// xc0 == 11000000
				return static_cast<Class>((octet.get() & 0xc0) >> 6);
			}
			inline Encoding get_encoding()
			{
				// x20=00100000
				return static_cast<Encoding>((octet.get() & 0x20) >> 5);
			}
			inline uint8_t get_tag()
			{
				// implicit cast
				// 0x1f mask = 00011111 binary
				return (octet.get() & 0x1f);
			}
			inline bool has_subsequent_octets()
			{
				return get_tag() >= 0x1f; // >=31
			}
			inline bool is_last_sequence_octet() { return octet.is_active<7>(); }

			inline bool is_valid_subsequent_octet()
			{
				return (octet.get() & 0x7f) != 0;
			}
			inline const bitset_t<8> getOctet()
			{
				return octet;
			}
		private:
			bitset_t<8> octet;
		};

		struct content
		{
		};

		template <size_t size>
		inline void encode(std::array<byte_t, size> &buff) {}

		inline void decode(std::istream &buff)
		{
			std::streampos buff_len = buff.tellg();
			// position at the begining
			buff.seekg(0U, std::ios_base::beg);
			// decode id headers
			byte_t _byte;
			uint64_t pos = 0;
			buff.read(reinterpret_cast<char *>(&_byte),
				sizeof(byte_t)); // read first byte

			Id id_1{ _byte };
			if (id_1.has_subsequent_octets())
			{
				std::vector<Id> ids;
				while (true)
				{
					pos++;
					buff.seekg(pos * sizeof(byte_t)); // move to next byte
					buff.read(reinterpret_cast<char *>(&_byte),
						sizeof(byte_t)); // read the byte
					ids.emplace_back({ _byte });
					if (!ids[pos].is_valid_subsequent_octet())
						throw;
					if (ids[pos].is_last_sequence_octet())
						break;
				}
				bitset_t<8> *bsVect = static_cast<bitset_t<8>*>(std::calloc(ids.size()*sizeof(bitset_t<8>));
				std::transform(std::begin(ids), std::end(ids), std::back_inserter(bsVect)
					, [](Id& id) { return id.getOctet(); }
				);
				auto result_bitset = join(std::begin(bsVect), std::end(bsVect), 1);
			}

			// read length
			pos++;
			buff.seekg(pos * sizeof(byte_t));
			buff.read(reinterpret_cast<char *>(&_byte), sizeof(byte_t));

			bitset_t<8> len{ static_cast<std::uint8_t>(_byte) };
			size_t content_len = len.get();
			if (id_1.get_encoding() != Id::Encoding::Primitive)
			{
				size_t const lenSize = (len.get() & 0x7f);
				bool indef_form = lenSize == 0;
				if (indef_form)
				{
					while (true)
					{
						pos++;
						buff.seekg(pos * sizeof(byte_t));
						buff.read(reinterpret_cast<char *>(&_byte), sizeof(byte_t));
						// TODO
					}
				}
				else
				{
					pos++;
					buff.seekg(pos * sizeof(byte_t));
					byte_t *bytes = static_cast<byte_t*> (std::calloc(lenSize, sizeof(byte_t)));
					buff.read(reinterpret_cast<char *>(&bytes[0]),
						lenSize * sizeof(byte_t));
					pos += lenSize;
					std::string bitStr;
					std::for_each(&bytes[0], &bytes[0] + lenSize, [&bitStr](byte_t &b) {
						bitStr.append(std::bitset<8>(static_cast<std::uint8_t>(b)).to_string());
					});
					content_len = bitset_t<details::compute_len<uint32_t>(lenSize * 8)>(bitStr).get();
				}
			}

			// read content

			//
		}
	}; // namespace lwcp

	// a) identifier octets (see 8.1.2);
	// b) length octets (see 8.1.3);
	// c) contents octets (see 8.1.4);
	// d) end-of-contents octets (see 8.1.5).

	struct asn_1_stream
	{
		asn_1_stream() = default;
		asn_1_stream(std::istream const &_input) : _binary_fbuffer(_input.rdbuf()) {}
		asn_1_stream(asn_1_stream &) = delete;

	private:
		std::istream _binary_fbuffer;
	};

} // namespace lwcp

#endif