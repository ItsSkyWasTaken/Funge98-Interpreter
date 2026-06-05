#ifndef FUNGE98_WORLD_HPP
#define FUNGE98_WORLD_HPP

#include <cstdint>
#include <iosfwd>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

#include "pointer.hpp"
#include "vector.hpp"

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
        static FungeWorld* fromFile(std::ifstream& file, int dim);

        /// Grabs the character at a specified location in the world. Returns a space \code ' '\endcode if there was no
        /// value specified in this location.
        ///
        /// @param location  the location to look at
        ///
        /// @return  the 32-bit character at that location
        [[nodiscard]] char32_t get(const Vector& location);

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

        /// Passes an argument supplied by the command line.
        ///
        /// @param arg  the argument to pass
        void passArg(const std::u32string& arg);

        /// Passes an environment variable specified by the system.
        ///
        /// @param  envar  the variable to pass
        void passEnvar(const std::u32string& envar);

        /// Starts the program.
        void run();

        /// Executes the instruction under the specified pointer and advances the pointer.
        ///
        /// @param ip  the pointer to receive the tick
        void tick(InstructionPointer& ip);

        /// Checks if an instruction pointer is within the bounds of the world, or if out of bounds, it checks for a
        /// travel direction towards the bounds of the world.
        ///
        /// @param ip  the pointer to check
        ///
        /// @return  true if the pointer is either in the world or traveling towards it (false if the pointer is out of
        ///          the world and traveling away from it)
        [[nodiscard]] bool boundsCheck(const InstructionPointer& ip) const;

        /// Checks if a temporary instruction pointer is within the bounds of the world, or if out of bounds, it checks
        /// for a travel direction towards the bounds of the world.
        ///
        /// @param position  the location to check
        /// @param delta     the delta to check with
        ///
        /// @return  true if the temporary pointer is either in the world or traveling towards it (false if the pointer
        ///          is out of the world and traveling away from it)
        [[nodiscard]] bool boundsCheck(const Vector& position, const Vector& delta) const;

        /// The number of dimensions that this world supports.
        const int32_t dimensions;

        /// Destructor. Deletes all the instruction pointers.
        ~FungeWorld();

    private:
        friend class InstructionSet;

        /// The lower bound of the active region, inclusive.
        Vector low;

        /// The upper bound of the active region, inclusive.
        Vector high;

        /// Whether writing files via the @code o@endcode command or any similar commands from fingerprints is allowed.
        /// Attempts to write files while this is disabled will result in a pointer reflection instead.
        bool write;

        /// Whether reading files via the @code i@endcode command or any similar commands from fingerprints is allowed.
        /// Attempts to read files while this is disabled will result in a pointer reflection instead.
        bool read;

        /// Whether executing system commands via the @code =@endcode command or any similar commands from fingerprints
        /// is allowed. Attempts to execute commands while this is disabled will result in a pointer reflection instead.
        bool execute;

        /// A map of all loaded chunks in this Funge world. Each chunk has 4096 characters (16kB). 2D chunks are 64×64
        /// and 3D chunks are 16×16×16.
        std::unordered_map<Vector, std::u32string> chunks;

        /// Gets the chunk and index associated with the specified coordinates.
        ///
        /// @param v  a set of coordinates to get
        ///
        /// @return   the chunk and index associated with the specified coordinates
        [[nodiscard]] std::pair<std::u32string&, int> getSublocation(const Vector& v);

        /// The queue of instruction pointers. Instruction pointers are polled, executed, and re-offered in a circle.
        std::queue<InstructionPointer*> pointers;

        /// A vector of arguments that were passed in when running this script via the terminal.
        std::vector<std::u32string> args;

        /// A vector of environment variables on the system.
        std::vector<std::u32string> envars;

        FungeWorld(std::unordered_map<Vector, std::u32string> chunks, const Vector& high);
};

/// Cleans up and exits the program with an exit code.
///
/// @param code  the exit code
void quit(int code);

#endif