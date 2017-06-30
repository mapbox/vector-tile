#ifndef VTZERO_GEOMETRY_HPP
#define VTZERO_GEOMETRY_HPP

#include <cstdint>

#include <protozero/pbf_reader.hpp>

#include "exception.hpp"
#include "types.hpp"

namespace vtzero {

    struct point {
        int32_t x;
        int32_t y;
    };

    inline bool operator==(const point& a, const point& b) noexcept {
        return a.x == b.x && a.y == b.y;
    }

    inline bool operator!=(const point& a, const point& b) noexcept {
        return !(a==b);
    }

    namespace detail {

        constexpr inline uint32_t command_integer(uint32_t id, uint32_t count) noexcept {
            return (id & 0x7) | (count << 3);
        }

        constexpr inline uint32_t command_move_to(uint32_t count = 0) noexcept {
            return command_integer(1, count);
        }

        constexpr inline uint32_t command_line_to(uint32_t count = 0) noexcept {
            return command_integer(2, count);
        }

        constexpr inline uint32_t command_close_path() noexcept {
            return command_integer(7, 0);
        }

        constexpr inline uint32_t get_command_id(uint32_t command_integer) noexcept {
            return command_integer & 0x7;
        }

        constexpr inline uint32_t get_command_count(uint32_t command_integer) noexcept {
            return command_integer >> 3;
        }

        inline constexpr int64_t det(const point& a, const point& b) noexcept {
            return int64_t(a.x) * int64_t(b.y) - int64_t(b.x) * int64_t(a.y);
        }

    } // namespace detail

    /**
     * Decode a geometry as specified in spec 4.3 from a sequence of 32 bit
     * unsigned integers.
     */
    class geometry_decoder {

        protozero::pbf_reader::const_uint32_iterator it;
        protozero::pbf_reader::const_uint32_iterator end;

        point m_cursor{0, 0};
        uint32_t m_command_id = 0;
        uint32_t m_count = 0;
        bool m_strict = true;

    public:

        geometry_decoder(const data_view& data, bool strict = true) :
            it(data.data(), data.data() + data.size()),
            end(data.data() + data.size(), data.data() + data.size()),
            m_strict(strict) {
        }

        uint32_t count() const noexcept {
            return m_count;
        }

        bool done() const noexcept {
            return it == end;
        }

        bool next_command(uint32_t expected_command) {
            if (it == end) {
                return false;
            }
            m_command_id = detail::get_command_id(*it);
            m_count = detail::get_command_count(*it);

            if (m_command_id != expected_command) {
                throw geometry_exception{};
            }

            ++it;

            return true;
        }

        point next_point() {
            assert(m_count > 0);

            if (it == end || std::next(it) == end) {
                throw geometry_exception{};
            }

            const uint32_t x = protozero::decode_zigzag32(*it++);
            const uint32_t y = protozero::decode_zigzag32(*it++);

            // spec 4.3.3.2 "For any pair of (dX, dY) the dX and dY MUST NOT both be 0."
            if (m_strict && x == 0 && y == 0) {
                throw geometry_exception{};
            }

            m_cursor.x += x;
            m_cursor.y += y;

            --m_count;

            return m_cursor;
        }

    }; // class geometry_decoder

} // namespace vtzero

#endif // VTZERO_GEOMETRY_HPP
