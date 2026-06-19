#ifndef FUNGE98_BASE_HPP
#define FUNGE98_BASE_HPP

#include <memory>

#include "fingerprint.hpp"

/// The fingerprint for bitwise operators.
class BooleanFP : public Fingerprint {
    public:
        /// Returns the static instance of this fingerprint, or creates a new one if it hasn't been created yet.
        ///
        /// @return  the singleton instance of this fingerprint
        static std::shared_ptr<BooleanFP> instance();

        BooleanFP(FingerprintToken);
        ~BooleanFP() override = default;

    // Overridden methods.
    protected:
        bool a(InstructionPointer& ip) override;
        bool n(InstructionPointer& ip) override;
        bool o(InstructionPointer& ip) override;
        bool x(InstructionPointer& ip) override;

    private:
        /// The singleton instance of this fingerprint.
        inline static std::shared_ptr<BooleanFP> inst = nullptr;
};

#endif