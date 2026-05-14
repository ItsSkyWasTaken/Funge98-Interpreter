#ifndef FUNGE98_WORLD_HPP
#define FUNGE98_WORLD_HPP

#include <cstdint>
#include <functional>
#include <iosfwd>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

#include "pointer.hpp"
#include "vector.hpp"
#include "world.hpp"

// Forward declaration
class FungeWorld;

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
};

/// The Funge World is where all the instructions and data are stored. It can be 1D, 2D, or 3D depending on how the
/// supplied file is set up or the passed arguments.
class FungeWorld {
    public:
        /// Loads a Funge world from a supplied file, automatically inferring the dimensions.
        ///
        /// @param file  an input file stream to the desired file
        ///
        /// @return  a direct object representation of the loaded Funge world
        static FungeWorld* fromFile(std::ifstream& file);

        /// Loads a Funge world from a supplied file, forcing a specific number of dimensions.
        ///
        /// @param file  an input file stream to the desired file
        /// @param dim   the number of dimensions the Funge world should have
        ///
        /// @return  a direct object representation of the loaded Funge world with the specified number of dimensions
        static FungeWorld* fromFile(std::ifstream& file, int8_t dim);

        /// Grabs the character at a specified location in the world. Returns a space \code ' '\endcode if there was no
        /// value specified in this location.
        ///
        /// @param location  the location to look at
        ///
        /// @return  the 32-bit character at that location
        [[nodiscard]] char32_t get(const Vector& location) const;

        /// Sets the data at the specified location to the supplied 32-bit data.
        ///
        /// @param location  the location to modify
        /// @param value     the new value to write
        void put(const Vector& location, char32_t value);

        /// Sets this world's permissions to read files.
        ///
        /// @param enabled  a boolean dictating whether this program should be able to read other files
        void setReadEnabled(bool enabled);

        /// Sets this world's permissions to create and modify files.
        ///
        /// @param enabled  a boolean dictating whether this program should be able to write files
        void setWriteEnabled(bool enabled);

        /// Sets this world's permissions to run system commands.
        ///
        /// @param enabled  a boolean dictating whether this program should be able to execute system commands
        void setExecuteEnabled(bool enabled);

        /// Checks this world's permissions to read files.
        ///
        /// @return  whether this program can read files
        [[nodiscard]] bool canRead() const;

        /// Checks this world's permissions to write files.
        ///
        /// @return  whether this program can write files
        [[nodiscard]] bool canWrite() const;

        /// Checks this world's permissions to run system commands.
        ///
        /// @return  whether this program can execute system commands
        [[nodiscard]] bool canExecute() const;

        /// Starts the program.
        void start();

        /// Executes the instruction under the next pointer in the queue and advances the pointer.
        void tick();

        /// Checks if an instruction pointer is within the bounds of the world, or if out of bounds, it checks for a
        /// travel direction towards the bounds of the world.
        ///
        /// @param ip  the pointer to check
        ///
        /// @return  true if the pointer is either in the world or traveling towards it (false if the pointer is out of
        ///          the world and traveling away from it)
        [[nodiscard]] bool boundsCheck(const InstructionPointer& ip) const;

        /// The number of dimensions that this world supports.
        const int8_t dimensions;

    private:
        friend class InstructionSet;

        // TODO: add more memory-efficient means of storing far away data;
        // ideally we do not need to create 2 billion 2D vectors of 4-byte characters to store some faraway data.

        /// The grid of data in the Funge world.
        std::vector<std::vector<std::u32string>> data;

        /// The queue of instruction pointers. Instruction pointers are polled, executed, and re-offered in a circle.
        std::queue<InstructionPointer*> pointers;

        /// The minimum X-values per row that have any data.
        std::vector<std::vector<int32_t>> xStarts;

        /// The minimum Y-values per plane that have any data.
        std::vector<int32_t> yStarts;

        /// The minimum Z-value that has any data.
        int32_t zStart;

        /// The lower bound of the active region, inclusive.
        Vector low;

        /// The upper bound of the active region, inclusive.
        Vector high;

        /// Whether writing files via the @code o@endcode command or any similar commands from fingerprints is allowed.
        /// Attempts to write files while this is disabled will result in a pointer reflect instead.
        bool write;

        /// Whether reading files via the @code i@endcode command or any similar commands from fingerprints is allowed.
        /// Attempts to read files while this is disabled will result in a pointer reflect instead.
        bool read;

        /// Whether executing system commands via the @code =@endcode command or any similar commands from fingerprints
        /// is allowed. Attempts to execute commands while this is disabled will result in a pointer reflect instead.
        bool execute;

        /// Private constructor. Instances of \code FungeWorld\endcode should be constructed via the static
        /// \code fromFile()\endcode methods.
        ///
        /// @param data  the 3D array of 32-bit characters obtained from the supplied file
        /// @param low   a vector showing the lower bound of the active region
        /// @param high  a vector showing the upper bound of the active region
        FungeWorld(const std::vector<std::vector<std::u32string>>& data, const Vector& low, const Vector& high);
};

/// Cleans up and exits the program with an exit code.
///
/// @param code  the exit code
void quit(int code);

#endif
