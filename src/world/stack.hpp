#ifndef FUNGE98_STACK_HPP
#define FUNGE98_STACK_HPP

#include <cstdint>
#include <stack>
#include <string>
#include <vector>
#include "vector.hpp"

class Stack {
    public:
        /// Initializes the global stack.
        static void initialize();

        /// Pushes a number to the TOSS.
        ///
        /// @param n  the number to push to the stack
        static inline void push(int32_t n);

        /// Pushes the ASCII number representation of a character to the TOSS.
        ///
        /// @param c  the character to push to the stack
        static inline void push(char c);

        /// Pushes a vector to the TOSS. The vector will be pushed as 1–3 separate cells, each component taking up one
        /// cell (i.e., 2D vectors will take two cells on the stack).
        ///
        /// @param v  the vector to push to the stack
        static void push(const Vector& v);

        /// Pushes a null-terminated string to the TOSS. The string will be pushed backwards onto the stack, taking up
        /// one cell per character, with a null character (0) at the beginning.
        ///
        /// For example, the string \code Hello\endcode would be pushed as \code 0 111 108 108 101 72\endcode (72 is at
        /// the top).
        ///
        /// @param s  the string to push to the stack
        static void push(const std::string& s);

        /// Removes and returns the top number of the TOSS.
        ///
        /// @return  the popped number, or 0 if the TOSS was empty
        static int32_t pop();

        /// Removes and returns the 8-bit ASCII character representation of the top number of the TOSS.
        ///
        /// @return  the popped number as an 8-bit ASCII character, or a null character if the TOSS was empty
        static char popChar();

        /// Removes and returns the top \e d elements from the TOSS and returns a \code Vector\endcode representation
        /// of the removed numbers. The last element removed corresponds to the \e x coordinate.
        ///
        /// @param dimensions  (\e d) the number of elements to remove, and the number of dimensions the returned
        ///                    vector will have
        ///
        /// @return  a \code Vector\endcode representation of the popped numbers; if the TOSS had less than \e d
        ///          elements, the remaining coordinates are filled with 0's from the left (e.g., returns
        ///          \code (0, y, z)\endcode for a 3D vector from only two elements
        static Vector popVector(int dimensions);

        /// Removes elements from the TOSS and converts them to characters until a 0 is hit, then returns a string
        /// representation of all the removed characters. If a 0 is never encountered, this call will simply empty the
        /// stack and return all characters as a string.
        ///
        /// @return  a string with all the removed characters from the TOSS, or an empty string \code ""\endcode if the
        ///          stack was empty
        static std::string popString();

    private:
        /// The global stack of stacks.
        static std::vector<Stack> globalStack;

        /// A stack of data. Each stack cell is 32 bits and can hold numbers or characters. Strings can be passed by
        /// taking up multiple consecutive cells, ending with a null character (0).
        std::stack<int32_t> stack;

        /// Gets a pointer to the topmost stack ("Top of Stack Stack").
        ///
        /// @return  a pointer to the topmost stack from the global stack
        inline static Stack* toss();

        /// Gets a pointer to the second stack on the top ("Second on Stack Stack").
        ///
        /// @return  a pointer to the second stack from the top of the global stack, or \code nullptr\endcode if there
        ///          is only one stack in the global stack
        inline static Stack* soss();
};

#endif