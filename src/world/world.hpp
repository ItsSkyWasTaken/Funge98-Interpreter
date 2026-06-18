#ifndef FUNGE98_WORLD_HPP
#define FUNGE98_WORLD_HPP

#include <iosfwd>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "vector.hpp"

class InstructionPointer;

/// The Funge World is where all the instructions and data are stored. It can be 1D, 2D, or 3D depending on how the
/// supplied file is set up or the passed arguments.
class FungeWorld {
    /// A private token that restricts creation to this class and allows this class to use smart pointers.
    struct FungeToken {
        explicit FungeToken() = default;
    };

    public:
        /// Loads a Funge world from a supplied file, automatically inferring the dimensions.
        ///
        /// @param file  an input file stream to the desired file
        ///
        /// @return  a direct object representation of the loaded Funge world
        static std::shared_ptr<FungeWorld> fromFile(std::ifstream& file);

        /// Loads a Funge world from a supplied file, forcing a specific number of dimensions.
        ///
        /// @param file  an input file stream to the desired file
        /// @param dim   the number of dimensions the Funge world should have
        ///
        /// @return  a direct object representation of the loaded Funge world with the specified number of dimensions
        static std::shared_ptr<FungeWorld> fromFile(std::ifstream& file, int dim);

        /// Constructs a Funge world. This constructor requires a FungeToken, which can only be created from this class.
        /// This allows construction in pointers while preventing construction outside of factory methods.
        ///
        /// @param chunks  the map of chunks in this world, in its initial state
        /// @param size    the size of the world
        FungeWorld(FungeToken, std::unordered_map<Vector, std::u32string> chunks, const Vector& size);

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
        /// @param ip_ptr  the pointer to receive the tick
        void tick(std::unique_ptr<InstructionPointer> ip_ptr);

        /// Gets the box of the active region of the world.
        ///
        /// @return  the two vectors representing the low and high corners of the active region, respectively
        std::pair<const Vector&, const Vector&> getBounds() const;

        /// Attempts to rebound an instruction pointer if it's out of bounds.
        ///
        /// @param ip  the pointer to rebound
        ///
        /// @return  two booleans, the first of which is true if it's able to rebound, and the second indicates if a
        ///          rebound took place
        std::pair<bool, bool> rebound(InstructionPointer& ip) const;

        /// Attempts to rebound a loose point according to a delta if it's out of bounds.
        ///
        /// @param location      the location of the point to rebound
        /// @param delta         the delta to rebound along
        /// @param inverseDelta  the inverse of the delta; precalculated so the same numbers can be reused without
        ///                      having to recalculate expensive division
        ///
        /// @return  two booleans, the first of which is true if the point is able to rebound, and the second indicates
        ///          if a rebound took place
        std::pair<bool, bool> rebound(Vector& location, const Vector& delta, const std::array<float, 3>& inverseDelta) const;

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
        /// @param v         a set of coordinates to get
        /// @param generate  whether to generate a new chunk as a result of this operation, if it doesn't exist
        ///
        /// @return   the chunk and index associated with the specified coordinates
        [[nodiscard]] std::pair<std::u32string*, int> getSublocation(const Vector& v, bool generate = false);

        /// The queue of instruction pointers. Instruction pointers are polled, executed, and re-offered in a circle.
        std::queue<std::unique_ptr<InstructionPointer>> pointers;

        /// A vector of arguments that were passed in when running this script via the terminal.
        std::vector<std::u32string> args;

        /// A vector of environment variables on the system.
        std::vector<std::u32string> envars;
};

/// Cleans up and exits the program with an exit code.
///
/// @param code  the exit code
void quit(int code);

#endif