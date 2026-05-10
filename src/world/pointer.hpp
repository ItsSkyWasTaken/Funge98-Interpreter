#ifndef FUNGE98_POINTER_HPP
#define FUNGE98_POINTER_HPP

#include "vector.hpp"

enum class PointerMode {
    /// The default state of the pointer. When the pointer steps onto a character in the Funge world in this state, it
    /// executes the command that corresponds to that character.
    NORMAL,

    /// "String mode": toggled via quotation marks. When the pointer steps onto a character in the Funge world in this
    /// state, it pushes that ASCII value onto the stack.
    STRING,

    /// "Comment mode": toggled via semicolons. Anything the pointer steps over while in this state is ignored, until it
    /// hits another semicolon.
    COMMENT
};

/// An @code InstructionPointer@endcode is an object that moves around the Funge world and executes the commands that it
/// walks over.
class InstructionPointer {
    public:
        /// Creates a new instruction pointer at the world's origin facing east, with a storage offset at the origin.
        ///
        /// @param dimensions  the number of dimensions in the world
        explicit InstructionPointer(int8_t dimensions);

        /// Creates a new instruction pointer at the specified location facing east, with a storage offset at the
        /// origin.
        ///
        /// @param location  the pointer's location
        explicit InstructionPointer(const Vector& location);

        /// Creates a new instruction pointer at the specified location and facing the specified direction, with a
        /// storage offset at the origin.
        ///
        /// @param location  the pointer's location
        /// @param delta     the direction this pointer is facing
        InstructionPointer(const Vector& location, const Vector& delta);

        /// Creates a new instruction pointer at the specified location and facing the specified direction, with the
        /// specified storage offset.
        ///
        /// @param location  the pointer's location
        /// @param delta     the direction this pointer is facing
        /// @param offset    the pointer's storage offset
        InstructionPointer(const Vector& location, const Vector& delta, const Vector& offset);

        /// Splits this instruction pointer, creating a new one.
        ///
        /// The new instruction pointer is an exact clone of the original, but with a reflected delta.
        ///
        /// @return  a pointer to the newly created instruction pointer
        [[nodiscard]] InstructionPointer* split() const;

        void advance();

        /// Updates this pointer's current mode.
        ///
        /// @param ipMode  the new PointerMode to update to
        inline void setPointerMode(PointerMode ipMode);

        /// Checks this pointer's current mode.
        ///
        /// @return the current mode of this pointer
        [[nodiscard]] inline PointerMode getPointerMode() const;

    private:
        /// The current position of this instruction pointer.
        Vector location;

        /// The current direction of travel of this instruction pointer.
        Vector delta;

        /// An offset for any \code p\endcode and \code g\endcode commands called by this instruction pointer.
        Vector storageOffset;

        /// The current mode that this instruction pointer is in.
        PointerMode mode;
};

#endif