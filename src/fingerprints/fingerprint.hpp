#ifndef FUNGE98_FINGERPRINT_HPP
#define FUNGE98_FINGERPRINT_HPP

#include <functional>
#include <memory>
#include <unordered_map>

class InstructionPointer;
class FungeWorld;

/// A fingerprint represents a set of extensions to the standard Funge98 instruction set. Each fingerprint implemented
/// by this interpreter simply extends this class and overrides the lettered methods.
class Fingerprint {
    protected:
        /// A protected token that restricts creation to this class and allows this class to use smart pointers.
        struct FingerprintToken {
            explicit FingerprintToken() = default;
        };

    public:
        /// A flag indicating that this fingerprint can have multiple instances created and destroyed throughout an
        /// app's lifetime. Transient fingerprints are destroyed when its associated pointer is destroyed. Non-transient
        const bool transient;

        /// Loads the fingerprint map.
        static void load(std::shared_ptr<FungeWorld> w);

        /// Gets the specified fingerprint instance.
        ///
        /// @param id  the ID of the fingerprint to load
        ///
        /// @return  a pointer to the specified fingerprint instance, or a null pointer if the ID does not map to a
        ///          fingerprint
        static std::shared_ptr<Fingerprint> load(int32_t id);

        /// Executes an overloaded instruction from the given instruction pointer.
        ///
        /// @param instruction  the instruction to execute
        /// @param ip           the instruction pointer that is to execute this instruction
        ///
        /// @return  a boolean determining whether the instruction successfully executed; a false value triggers a
        /// pointer reflection
        bool execute(char32_t instruction, InstructionPointer& ip);

        /// Returns a set of characters that this fingerprint overrides, to allow for easy loading.
        ///
        /// @return  the set of characters overloaded by this fingerprint
        const std::vector<char32_t>& getImplementedInstructions() const {
            return implementedInstructions;
        }

        /// Creates a fingerprint instance.
        ///
        /// @param transient  whether this fingerprint can have multiple instances (one per IP); false if this is a
        ///                   singleton (any fingerprint that doesn't have states should be singletons)
        Fingerprint(FingerprintToken, const bool transient): transient(transient){}

        virtual ~Fingerprint() = 0;

    protected:
        /// A static pointer to the Funge world.
        inline static std::shared_ptr<FungeWorld> world;

        /// A list of all instructions, alphabetized.
        const std::vector<std::function<bool(Fingerprint*, InstructionPointer&)>> instructions = {
            &Fingerprint::a, &Fingerprint::b, &Fingerprint::c,
            &Fingerprint::d, &Fingerprint::e, &Fingerprint::f,
            &Fingerprint::g, &Fingerprint::a, &Fingerprint::i,
            &Fingerprint::j, &Fingerprint::k, &Fingerprint::l,
            &Fingerprint::m, &Fingerprint::n, &Fingerprint::o,
            &Fingerprint::p, &Fingerprint::q, &Fingerprint::r,
            &Fingerprint::s, &Fingerprint::t, &Fingerprint::u,
            &Fingerprint::v, &Fingerprint::w, &Fingerprint::x,
            &Fingerprint::y, &Fingerprint::z
        };

        /// A list of instructions overloaded by this fingerprint.
        std::vector<char32_t> implementedInstructions;

        /// Executes the instruction associated with 'A'.
        ///
        /// @param ip  the instruction pointer to execute from
        ///
        /// @return  a boolean determining whether the instruction successfully executed; a false value triggers a
        ///          pointer reflection
        virtual bool a(InstructionPointer& ip);

        /// Executes the instruction associated with 'B'.
        ///
        /// @param ip  the instruction pointer to execute from
        ///
        /// @return  a boolean determining whether the instruction successfully executed; a false value triggers a
        ///          pointer reflection
        virtual bool b(InstructionPointer& ip);

        /// Executes the instruction associated with 'C'.
        ///
        /// @param ip  the instruction pointer to execute from
        ///
        /// @return  a boolean determining whether the instruction successfully executed; a false value triggers a
        ///          pointer reflection
        virtual bool c(InstructionPointer& ip);

        /// Executes the instruction associated with 'D'.
        ///
        /// @param ip  the instruction pointer to execute from
        ///
        /// @return  a boolean determining whether the instruction successfully executed; a false value triggers a
        ///          pointer reflection
        virtual bool d(InstructionPointer& ip);

        /// Executes the instruction associated with 'E'.
        ///
        /// @param ip  the instruction pointer to execute from
        ///
        /// @return  a boolean determining whether the instruction successfully executed; a false value triggers a
        ///          pointer reflection
        virtual bool e(InstructionPointer& ip);

        /// Executes the instruction associated with 'F'.
        ///
        /// @param ip  the instruction pointer to execute from
        ///
        /// @return  a boolean determining whether the instruction successfully executed; a false value triggers a
        ///          pointer reflection
        virtual bool f(InstructionPointer& ip);

        /// Executes the instruction associated with 'G'.
        ///
        /// @param ip  the instruction pointer to execute from
        ///
        /// @return  a boolean determining whether the instruction successfully executed; a false value triggers a
        ///          pointer reflection
        virtual bool g(InstructionPointer& ip);

        /// Executes the instruction associated with 'H'.
        ///
        /// @param ip  the instruction pointer to execute from
        ///
        /// @return  a boolean determining whether the instruction successfully executed; a false value triggers a
        ///          pointer reflection
        virtual bool h(InstructionPointer& ip);

        /// Executes the instruction associated with 'I'.
        ///
        /// @param ip  the instruction pointer to execute from
        ///
        /// @return  a boolean determining whether the instruction successfully executed; a false value triggers a
        ///          pointer reflection
        virtual bool i(InstructionPointer& ip);

        /// Executes the instruction associated with 'J'.
        ///
        /// @param ip  the instruction pointer to execute from
        ///
        /// @return  a boolean determining whether the instruction successfully executed; a false value triggers a
        ///          pointer reflection
        virtual bool j(InstructionPointer& ip);

        /// Executes the instruction associated with 'K'.
        ///
        /// @param ip  the instruction pointer to execute from
        ///
        /// @return  a boolean determining whether the instruction successfully executed; a false value triggers a
        ///          pointer reflection
        virtual bool k(InstructionPointer& ip);

        /// Executes the instruction associated with 'L'.
        ///
        /// @param ip  the instruction pointer to execute from
        ///
        /// @return  a boolean determining whether the instruction successfully executed; a false value triggers a
        ///          pointer reflection
        virtual bool l(InstructionPointer& ip);

        /// Executes the instruction associated with 'M'.
        ///
        /// @param ip  the instruction pointer to execute from
        ///
        /// @return  a boolean determining whether the instruction successfully executed; a false value triggers a
        ///          pointer reflection
        virtual bool m(InstructionPointer& ip);

        /// Executes the instruction associated with 'N'.
        ///
        /// @param ip  the instruction pointer to execute from
        ///
        /// @return  a boolean determining whether the instruction successfully executed; a false value triggers a
        ///          pointer reflection
        virtual bool n(InstructionPointer& ip);

        /// Executes the instruction associated with 'O'.
        ///
        /// @param ip  the instruction pointer to execute from
        ///
        /// @return  a boolean determining whether the instruction successfully executed; a false value triggers a
        ///          pointer reflection
        virtual bool o(InstructionPointer& ip);

        /// Executes the instruction associated with 'P'.
        ///
        /// @param ip  the instruction pointer to execute from
        ///
        /// @return  a boolean determining whether the instruction successfully executed; a false value triggers a
        ///          pointer reflection
        virtual bool p(InstructionPointer& ip);

        /// Executes the instruction associated with 'Q'.
        ///
        /// @param ip  the instruction pointer to execute from
        ///
        /// @return  a boolean determining whether the instruction successfully executed; a false value triggers a
        ///          pointer reflection
        virtual bool q(InstructionPointer& ip);

        /// Executes the instruction associated with 'R'.
        ///
        /// @param ip  the instruction pointer to execute from
        ///
        /// @return  a boolean determining whether the instruction successfully executed; a false value triggers a
        ///          pointer reflection
        virtual bool r(InstructionPointer& ip);

        /// Executes the instruction associated with 'S'.
        ///
        /// @param ip  the instruction pointer to execute from
        ///
        /// @return  a boolean determining whether the instruction successfully executed; a false value triggers a
        ///          pointer reflection
        virtual bool s(InstructionPointer& ip);

        /// Executes the instruction associated with 'T'.
        ///
        /// @param ip  the instruction pointer to execute from
        ///
        /// @return  a boolean determining whether the instruction successfully executed; a false value triggers a
        ///          pointer reflection
        virtual bool t(InstructionPointer& ip);

        /// Executes the instruction associated with 'U'.
        ///
        /// @param ip  the instruction pointer to execute from
        ///
        /// @return  a boolean determining whether the instruction successfully executed; a false value triggers a
        ///          pointer reflection
        virtual bool u(InstructionPointer& ip);

        /// Executes the instruction associated with 'V'.
        ///
        /// @param ip  the instruction pointer to execute from
        ///
        /// @return  a boolean determining whether the instruction successfully executed; a false value triggers a
        ///          pointer reflection
        virtual bool v(InstructionPointer& ip);

        /// Executes the instruction associated with 'W'.
        ///
        /// @param ip  the instruction pointer to execute from
        ///
        /// @return  a boolean determining whether the instruction successfully executed; a false value triggers a
        ///          pointer reflection
        virtual bool w(InstructionPointer& ip);

        /// Executes the instruction associated with 'X'.
        ///
        /// @param ip  the instruction pointer to execute from
        ///
        /// @return  a boolean determining whether the instruction successfully executed; a false value triggers a
        ///          pointer reflection
        virtual bool x(InstructionPointer& ip);

        /// Executes the instruction associated with 'Y'.
        ///
        /// @param ip  the instruction pointer to execute from
        ///
        /// @return  a boolean determining whether the instruction successfully executed; a false value triggers a
        ///          pointer reflection
        virtual bool y(InstructionPointer& ip);

        /// Executes the instruction associated with 'Z'.
        ///
        /// @param ip  the instruction pointer to execute from
        ///
        /// @return  a boolean determining whether the instruction successfully executed; a false value triggers a
        ///          pointer reflection
        virtual bool z(InstructionPointer& ip);

    private:
        /// A map of integers with their associated fingerprints.
        inline static std::unordered_map<int32_t, std::function<std::shared_ptr<Fingerprint>()>> fingerprints;
};

#endif