#ifndef FUNGE98_INSTRUCTIONS_HPP
#define FUNGE98_INSTRUCTIONS_HPP

#include <functional>
#include <unordered_map>

#include "world.hpp"

/// A static utility class for hosting the available commands in Funge98.
class InstructionSet {
    public:
        /// Loads the command map and links the world.
        ///
        /// @param w  the world to link
        static void load(FungeWorld& w);

        /// Executes the command at the pointer's location.
        ///
        /// @param ip  a reference to the pointer that should execute the command
        static bool execute(InstructionPointer& ip);

        /// Check if a character has a mapped command.
        ///
        /// @param command  the character to check
        static bool supports(char32_t command);

        // Utility class; prevent instantiation of object
        InstructionSet() = delete;
        InstructionSet(const InstructionSet&) = delete;

    private:
        /// A map of all the commands available. Commands take a reference of the instruction pointer that executed them
        /// as their sole parameter, and they return a boolean indicating their success status. An unsuccessful
        /// execution triggers a pointer reflect.
        static std::unordered_map<char32_t, std::function<bool(InstructionPointer&)>> commands;

        /// A pointer to the world.
        static FungeWorld* world;

        /// The column position of the text caret in the terminal. This facilitates deleting malformed input from
        /// requests for numbers (which in turn facilitates doing more creative stuff with terminal art and graphics).
        static int pointerPosition;

        /// A number related to ANSI flags. \code 0\endcode if nothing is happening, \code 1\endcode if an escape
        /// character has just been printed, and \code 2\endcode after the open bracket is printed but before the
        /// closing character is printed.
        static int ansiFlag;

        /// The parameter passed into the ANSI escape sequence, used to determine how far, if at all, the text caret
        /// moved as a result of the ANSI sequence.
        static int ansiParameter;
};

#endif