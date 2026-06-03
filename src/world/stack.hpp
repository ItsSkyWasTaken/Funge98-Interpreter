#ifndef FUNGE98_STACK_HPP
#define FUNGE98_STACK_HPP

#include <cstdint>
#include <stack>
#include <stdfloat>
#include <string>
#include <vector>

#include "vector.hpp"

/// The Stack of Funge98 consists of substacks, each carrying 32-bit cells. Values pushed to the stack automatically
/// conform to the 32-bit structure.
class Stack {
    public:
        /// Creates a stack of stacks of 32-bit datasets, with one single empty stack already initialized.
        Stack();

        /// Pushes a number to the TOSS.
        ///
        /// @param n  the number to push to the stack
        void push(int32_t n);

        /// Pushes the ASCII number representation of a character to the TOSS.
        ///
        /// @param c  the character to push to the stack
        void push(char32_t c);

        /// Pushes a 32-bit word representation of a single-precision floating point number to the TOSS.
        ///
        /// @param f  the floating point number to push to the stack
        void push(std::float32_t f);

        /// Pushes a 64-bit double-precision floating point number as two 32-bit words to the TOSS.
        ///
        /// The lower 32 bits will be at the top of the stack.
        ///
        /// @param d  the floating point number to push to the stack
        void push(std::float64_t d);

        /// Pushes a vector to the TOSS. The vector will be pushed as 1–3 separate cells, each component taking up one
        /// cell (i.e., 2D vectors will take two cells on the stack).
        ///
        /// @param v  the vector to push to the stack
        void push(const Vector& v);

        /// Pushes a null-terminated string to the TOSS. The string will be pushed backwards onto the stack, taking up
        /// one cell per character, with a null character (0) at the beginning.
        ///
        /// For example, the string \code Hello\endcode would be pushed as \code 0 111 108 108 101 72\endcode (72 is at
        /// the top).
        ///
        /// @param s  the string to push to the stack
        void push(const std::u32string& s);

        /// Gets a copy of the top element as an integer.
        ///
        /// @return  a copy of the top integer value of the stack
        int32_t peek();

        /// Duplicates the top element of the TOSS.
        void duplicate();

        /// Clears the TOSS.
        void clear();

        /// Gets the number of stacks on the stack of stacks.
        ///
        /// @return  the number of stacks on the stack stack
        [[nodiscard]] uint32_t size() const;

        /// Removes and returns the top number of the TOSS.
        ///
        /// @return  the popped number, or 0 if the TOSS was empty
        int32_t pop();

        /// Removes and returns the 8-bit ASCII character representation of the top number of the TOSS.
        ///
        /// @return  the popped number as an 8-bit ASCII character, or a null character if the TOSS was empty
        char32_t popChar();

        /// Removes and returns the top 32 bits of the TOSS as a single-precision floating point number.
        ///
        /// @return  the single-precision floating point number reinterpretation of the popped 32-bit number
        std::float32_t popFloat();

        /// Pops the top two 32-bit numbers from the TOSS and reinterprets the bits into a 64-bit double precision
        /// number. The first number popped will be the lower 32 bits, and the second number will be the upper bits.
        ///
        /// @return  the double-precision floating point number reinterpretation of the two popped 32-bit numbers
        std::float64_t popDouble();

        /// Removes and returns the top \e d elements from the TOSS and returns a \code Vector\endcode representation
        /// of the removed numbers. The last element removed corresponds to the \e x coordinate.
        ///
        /// @param dimensions  (\e d) the number of elements to remove, and the number of dimensions the returned
        ///                    vector will have
        ///
        /// @return  a \code Vector\endcode representation of the popped numbers; if the TOSS had less than \e d
        ///          elements, the remaining coordinates are filled with 0's from the left (e.g., returns
        ///          \code (0, y, z)\endcode for a 3D vector from only two elements
        Vector popVector(int dimensions);

        /// Removes elements from the TOSS and converts them to characters until a 0 is hit, then returns a string
        /// representation of all the removed characters. If a 0 is never encountered, this call will simply empty the
        /// stack and return all characters as a string.
        ///
        /// @return  a string with all the removed characters from the TOSS, or an empty string \code ""\endcode if the
        ///          stack was empty
        std::u32string popString();

        /// Creates a new stack on top of the TOSS (making the former TOSS now the SOSS, while the new stack is the
        /// TOSS), transferring \e e elements from the TOSS to the SOSS, preserving order.
        ///
        /// If \e e is negative, zeros are pushed onto the current SOSS instead.
        ///
        /// If \e is greater than the size of the SOSS, the remaining space is filled with zeros.
        ///
        /// @param elements  the number of elements to transfer from the SOSS to the TOSS after the split
        void split(int32_t elements);

        /// Transfers \e e elements from the TOSS to the SOSS, then destroys the TOSS, making the former SOSS the new
        /// TOSS.
        ///
        /// If \e is negative, cells are popped off of the (current) SOSS instead.
        ///
        /// @param elements  the number of elements to transfer from the TOSS to the SOSS before the collapse
        void collapse(int32_t elements);

        /// Transfers \e e elements from the SOSS to the TOSS. If \e e is negative, elements are transferred from the
        /// TOSS to the SOSS instead.
        ///
        /// @param elements  the number of elements to transfer
        void transfer(int32_t elements);

    private:
        friend class InstructionPointer;

        /// The stack of stacks.
        std::vector<std::stack<int32_t>> ss;

        /// Gets a pointer to the topmost stack ("Top of Stack Stack").
        ///
        /// @return  a pointer to the topmost stack from the main stack
        std::stack<int32_t>* toss();

        /// Gets a pointer to the second stack on the top ("Second on Stack Stack").
        ///
        /// @return  a pointer to the second stack from the top of the global stack, or \code nullptr\endcode if there
        ///          is only one stack in the main stack
        std::stack<int32_t>* soss();
};

#endif