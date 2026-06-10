#ifndef FUNGE98_VECTOR_HPP
#define FUNGE98_VECTOR_HPP

#include <cstddef>
#include <cstdint>
#include <functional>

// Forward declaration so the class and hash struct can interact with each other.
class Vector;

template<> struct std::hash<Vector> {
    std::size_t operator()(const Vector& v) const noexcept;
};

/// A @code Vector@endcode represents a set of coordinates. These coordinates may represent a location or a direction
/// and may be absolute or relative, depending on the context in which they are used.
class Vector {
    public:
        /// Generates a default location vector of the specified dimension.
        ///
        /// Locations default at the origin (i.e., all coordinates are 0).
        ///
        /// @param dim  the number of dimensions for the location vector
        ///
        /// @return  a default location vector of the given dimension
        static Vector origin(int32_t dim);

        /// Generates a default delta vector of the specified dimension.
        ///
        /// Delta vectors default to pointing east; that is, their x-coordinate is 1 while the extra coordinates are 0.
        ///
        /// @param dim  the number of dimensions for the delta vector
        ///
        /// @return  a default delta vector of the given dimension
        static Vector east(int32_t dim);

        /// Generates a delta vector that points one unit in a random cardinal direction.
        ///
        /// @param dim  the number of dimensions the delta vector should have
        ///
        /// @return  the randomly generated delta vector
        static Vector random(int32_t dim);

        /// Constructs a one-dimensional vector with the specified coordinate.
        ///
        /// @param x  the x-coordinate of the vector
        Vector(int x);

        /// Constructs a two-dimensional vector with the specified coordinates.
        ///
        /// @param x  the x-coordinate of the vector
        /// @param y  the y-coordinate of the vector
        Vector(int x, int y);

        /// Constructs a three-dimensional vector with the specified coordinates.
        ///
        /// @param x  the x-coordinate of the vector
        /// @param y  the y-coordinate of the vector
        /// @param z  the z-coordinate of the vector
        Vector(int x, int y, int z);

        /// Adds another vector to this vector and returns the result.
        ///
        /// The two vectors should have the same dimensions. If the first vector has fewer dimensions, the available
        /// coordinates are added and the rest are truncated. If the second vector has fewer dimensions, the missing
        /// dimensions are filled with zeros.
        ///
        /// @param vector  the vector to add to this vector
        ///
        /// @result  the sum of the two vectors as a new @code Vector@endcode object
        Vector operator+(const Vector& vector) const;

        /// Subtracts another vector from this vector and returns the result.
        ///
        /// The two vectors should have the same dimensions. If the first vector has fewer dimensions, the available
        /// coordinates are subtracted and the rest are truncated. If the second vector has fewer dimensions,
        /// the missing dimensions are filled with zeros.
        ///
        /// @param vector  the vector to subtract from this vector
        ///
        /// @result  the difference of the two vectors as a new @code Vector@endcode object
        Vector operator-(const Vector& vector) const;

        /// Multiplies this vector by a scalar value and returns the result.
        ///
        /// The new vector will have all of its components multiplied by the scalar compared to the original vector.
        ///
        /// @param scalar  the scalar value to multiply this vector by
        ///
        /// @return the product as a new @code Vector@endcode object
        Vector operator*(int scalar) const;

        /// Multiplies a scalar by a vector and returns the result.
        ///
        /// The new vector will have all of its components multiplied by the scalar compared to the original vector.
        ///
        /// @param scalar  the scalar value to multiply by
        /// @param vector  the vector to multiply
        ///
        /// @return the product as a new @code Vector@endcode object
        friend Vector operator*(int scalar, const Vector& vector);

        /// Returns a copy of this vector, but reflected.
        ///
        /// @return  a new @code Vector@endcode object representation of this vector reflected
        Vector operator-() const;

        /// Adds another vector directly to this vector.
        ///
        /// @param vector  the vector to add to this vector
        void operator+=(const Vector& vector);

        /// Subtracts another vector directly from this vector.
        ///
        /// @param vector  the vector to subtract from this vector
        void operator-=(const Vector& vector);

        /// Multiplies a scalar value directly to this vector.
        ///
        /// @param scalar  the scalar to multiply to this vector
        void operator*=(int scalar);

        /// Sets the vector's coordinates to those of the new vector.
        ///
        /// @param v  the new vector
        ///
        /// @return  a reference to this vector
        Vector& operator=(const Vector& v);

        /// Checks for equality by comparing the coordinates.
        ///
        /// @param v  the vector to compare against this vector
        ///
        /// @return  \code true\endcode if the coordinates are equal
        bool operator==(const Vector& v) const;

        /// The number of dimensions on this vector.
        int32_t dimensions;

        /// Gets the X-coordinate of this vector.
        ///
        /// @return  the vector's X-coordinate
        [[nodiscard]] int32_t getX() const;

        /// Gets the Y-coordinate of this vector.
        ///
        /// @return  the vector's Y-coordinate
        [[nodiscard]] int32_t getY() const;

        /// Gets the Z-coordinate of this vector.
        ///
        /// @return  the vector's Z-coordinate
        [[nodiscard]] int32_t getZ() const;

    private:
        friend struct std::hash<Vector>;

        /// The x-coordinate of the vector.
        int32_t x;

        /// The y-coordinate of the vector.
        int32_t y;

        /// The z-coordinate of the vector.
        int32_t z;
};

#endif