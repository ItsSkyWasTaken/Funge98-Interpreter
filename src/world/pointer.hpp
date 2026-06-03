#ifndef FUNGE98_POINTER_HPP
#define FUNGE98_POINTER_HPP

#include "stack.hpp"
#include "vector.hpp"

/// An enum of different possible states of the pointer. These states dictate what happens when a pointer passes over an
/// instruction in the Funge world.
enum class PointerState {
    /// The default state of the pointer. When the pointer steps onto a character in the Funge world in this state, it
    /// executes the command that corresponds to that character.
    NORMAL,

    /// "String mode": toggled via quotation marks. When the pointer steps onto a character in the Funge world in this
    /// state, it pushes that ASCII value onto the stack.
    STRING,

    /// "String mode", but spaces are ignored. This state is used for whitespace collapsing.
    STRING_IGNORE_SPACES,

    /// The pointer stops and exits the world at the end of the tick.
    EXITING
};

/// An @code InstructionPointer@endcode is an object that moves around the Funge world and executes the commands that it
/// walks over.
class InstructionPointer {
    public:
        /// Creates a new instruction pointer at the world's origin facing east, with a storage offset at the origin and
        /// an empty stack.
        ///
        /// @param dimensions  the number of dimensions in the world
        explicit InstructionPointer(int32_t dimensions);

        /// Creates a new instruction pointer at the specified location facing east, with a storage offset at the
        /// origin and an empty stack.
        ///
        /// @param location  the pointer's location
        explicit InstructionPointer(const Vector& location);

        /// Creates a new instruction pointer at the specified location and facing the specified direction, with a
        /// storage offset at the origin and an empty stack.
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
        /// @param stack     the stack associated with this pointer
        InstructionPointer(const Vector& location, const Vector& delta, const Vector& offset, const Stack* stack);

        /// Splits this instruction pointer, creating a new one.
        ///
        /// The new instruction pointer is an exact clone of the original, but with a reflected delta.
        ///
        /// @return  a pointer to the newly created instruction pointer
        [[nodiscard]] InstructionPointer* split() const;

        /// Advances this pointer one step according to its delta vector.
        void advance(int steps);

        /// Gets a read-only reference to this instruction pointer's location vector.
        ///
        /// @return  the current position of this instruction pointer
        [[nodiscard]] const Vector& getLocation() const;

        /// Gets a read-only reference to this instruction pointer's delta (travel direction) vector.
        ///
        /// @return  the current delta of this instruction pointer
        [[nodiscard]] const Vector& getDelta() const;

        /// Gets a read-only reference to this instruction pointer's storage offset vector.
        ///
        /// @return  the current storage offset of this instruction pointer
        [[nodiscard]] const Vector& getOffset() const;

        /// Sets this pointer's current location.
        ///
        /// @param v  the new location
        void setLocation(const Vector& v);

        /// Sets this pointer's current location.
        ///
        /// @param x  the new location's x-coordinate
        /// @param y  the new location's y-coordinate
        /// @param z  the new location's z-coordinate
        void setLocation(int32_t x, int32_t y, int32_t z);

        /// Sets this pointer's current delta.
        ///
        /// @param v  the new delta
        void setDelta(const Vector& v);

        /// Sets this pointer's current delta.
        ///
        /// @param x  the new delta's x-coordinate
        /// @param y  the new delta's y-coordinate
        /// @param z  the new delta's z-coordinate
        void setDelta(int32_t x, int32_t y, int32_t z);

        /// Updates this pointer's current mode.
        ///
        /// @param ipMode  the new PointerMode to update to
        void setPointerState(PointerState ipMode);

        /// Checks this pointer's current mode.
        ///
        /// @return the current mode of this pointer
        [[nodiscard]] PointerState getPointerState() const;

        /// Gets a reference to this pointer's stack.
        ///
        /// @return  this pointer's stack
        [[nodiscard]] Stack& getStack() const;

        /// Starts a new block.
        void startBlock();

        /// Ends the last started block.
        void endBlock();

        /// Gets the sizes of each stack, starting from the TOSS and ending with the BOSS.
        [[nodiscard]] std::vector<int32_t> stackSizes() const;

        /// The unique id of this pointer.
        const uint32_t id;

        /// Destructor. Deletes the associated stack.
        ~InstructionPointer();

    private:
        /// The ID of the next pointer.
        static uint32_t nextId;

        /// The current position of this instruction pointer.
        Vector location;

        /// The current direction of travel of this instruction pointer.
        Vector delta;

        /// An offset for any \code p\endcode and \code g\endcode commands called by this instruction pointer.
        Vector storageOffset;

        /// The stack associated with this pointer.
        Stack* const stack;

        /// The current mode that this instruction pointer is in.
        PointerState mode;
};

#endif