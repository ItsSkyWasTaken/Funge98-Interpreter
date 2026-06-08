#ifndef FUNGE98_STRN_HPP
#define FUNGE98_STRN_HPP

#include <memory>

#include "fingerprint.hpp"

/// The string fingerprint.
class StringFP : public Fingerprint {
    public:
        /// Returns the static instance of this fingerprint, or creates a new one if it hasn't been created yet.
        ///
        /// @return  the singleton instance of this fingerprint
        static std::shared_ptr<StringFP> instance();

        StringFP(FingerprintToken);
        ~StringFP() override = default;

    // Overridden methods.
    protected:
        bool a(InstructionPointer& ip) override;
        bool c(InstructionPointer& ip) override;
        bool d(InstructionPointer& ip) override;
        bool f(InstructionPointer& ip) override;
        bool g(InstructionPointer& ip) override;
        bool i(InstructionPointer& ip) override;
        bool l(InstructionPointer& ip) override;
        bool m(InstructionPointer& ip) override;
        bool n(InstructionPointer& ip) override;
        bool p(InstructionPointer& ip) override;
        bool r(InstructionPointer& ip) override;
        bool s(InstructionPointer& ip) override;
        bool v(InstructionPointer& ip) override;

    private:
        /// The singleton instance of this fingerprint.
        inline static std::shared_ptr<StringFP> inst = nullptr;
};

#endif
